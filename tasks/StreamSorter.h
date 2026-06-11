#ifndef LAB4_STREAMSORTER_H
#define LAB4_STREAMSORTER_H

#include <functional> // std::function — сериализаторы, Compare
#include <string> // std::string — пути временных файлов
#include <cstdio> // std::remove — удалить временный файл
#include "../my_except.h"
#include "../atd/BinaryHeap.h"
#include "../core/DynamicArray.h"
#include "../core/Streams/ParentsStream.h"
#include "../core/Streams/ReadStreams.h"
#include "../core/Streams/WriteStreams.h"

// Вариант 10.2: сортировка потока с помощью бинарной кучи.
// Два режима:
//   SortInMemory — весь поток влезает в память: куча целиком + извлечение по порядку
//   SortExternal — поток большой: режем на отсортированные куски во временные файлы,
//                  потом k-путевое слияние через кучу (в памяти только chunkSize элементов + головы кусков)
// сортировать можно только конечный поток
template <class T, class Compare = std::less<T>>
class StreamSorter {
private:
    //голова куска при слиянии: текущее значение + из какого куска оно пришло
    struct ChunkHead {
        T value;
        int chunkIndex;
    };
    //сравниваем головы по значению (тем же Compare, что и сортировка)
    struct HeadCompare {
        Compare cmp;
        bool operator()(const ChunkHead& a, const ChunkHead& b) const {
            return cmp(a.value, b.value);
        }
    };

public:
    //весь поток в кучу, потом извлекаем по порядку — пирамидальная сортировка
    static void SortInMemory(ReadOnlyStream<T>& in, WriteOnlyStream<T>& out) {
        if (!in.IsFinite())
            throw InvalidArgument("бесконечный поток нельзя отсортировать");
        BinaryHeap<T, Compare> heap;
        while (!in.IsEndOfStream())
            heap.Insert(in.Read());
        while (!heap.IsEmpty())
            out.Write(heap.ExtractMinOrMax());
    }

    //внешняя сортировка: для потоков, не влезающих в память
    static void SortExternal(ReadOnlyStream<T>& in, WriteOnlyStream<T>& out, size_t chunkSize,
                             std::function<std::string(const T&)> serialize,
                             std::function<T(const std::string&)> deserialize,
                             const std::string& tmpDir = "/tmp") {
        if (chunkSize == 0)
            throw InvalidArgument("chunkSize");
        if (!in.IsFinite())
            throw InvalidArgument("бесконечный поток нельзя отсортировать");

        // фаза 1: режем вход на куски по chunkSize, каждый сортируем кучей
        // и сбрасываем в отдельный временный файл
        DynamicArray<std::string> chunkFiles;
        int chunkCount = 0;
        while (!in.IsEndOfStream()) {
            BinaryHeap<T, Compare> chunk;
            for (size_t i = 0; i < chunkSize && !in.IsEndOfStream(); i++)
                chunk.Insert(in.Read());

            std::string path = tmpDir + "/lab4_sort_chunk_" + std::to_string(chunkCount) + ".txt";
            FileWriteStream<T> writer(path, serialize);
            writer.Open();
            while (!chunk.IsEmpty())
                writer.Write(chunk.ExtractMinOrMax()); // кусок выходит отсортированным
            writer.Close();

            chunkFiles.Resize(chunkCount + 1);
            chunkFiles.Set(chunkCount, path);
            chunkCount++;
        }

        // фаза 2: k-путевое слияние — в куче по 1 голове от каждого куска;
        // достаём минимум, пишем в выход, подтягиваем следующий из того же куска
        DynamicArray<FileReadStream<T>*> chunkReaders(chunkCount);
        for (int i = 0; i < chunkCount; i++) {
            chunkReaders.Set(i, new FileReadStream<T>(chunkFiles.Get(i), deserialize));
            chunkReaders.Get(i)->Open();
        }

        BinaryHeap<ChunkHead, HeadCompare> heads;
        for (int i = 0; i < chunkCount; i++)
            if (!chunkReaders.Get(i)->IsEndOfStream())
                heads.Insert(ChunkHead{chunkReaders.Get(i)->Read(), i});

        while (!heads.IsEmpty()) {
            ChunkHead head = heads.ExtractMinOrMax();
            out.Write(head.value);
            if (!chunkReaders.Get(head.chunkIndex)->IsEndOfStream()) // подтянуть следующий из того же куска
                heads.Insert(ChunkHead{chunkReaders.Get(head.chunkIndex)->Read(), head.chunkIndex});
        }

        // прибраться: закрыть потоки и удалить временные файлы
        for (int i = 0; i < chunkCount; i++) {
            chunkReaders.Get(i)->Close();
            delete chunkReaders.Get(i);
            std::remove(chunkFiles.Get(i).c_str());
        }
    }
};

#endif //LAB4_STREAMSORTER_H
