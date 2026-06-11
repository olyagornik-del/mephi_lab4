#ifndef LAB4_WRITESTREAMS_H
#define LAB4_WRITESTREAMS_H

#include <functional> // std::function — тип сериализатора
#include <string> // std::string — имя файла, строка-результат сериализации
#include <fstream> // std::ofstream — запись файла
#include "ParentsStream.h"
#include "../Sequences/MutableArraySequence.h"

// запись в последовательность в памяти; результат можно забрать через GetResult
template <class T>
class SequenceWriteStream : public WriteOnlyStream<T> {
private:
    MutableArraySequence<T> data; // накопленные элементы
public:
    SequenceWriteStream(): data() {}

    size_t GetPosition() const override { return (size_t)data.GetLength(); }
    size_t Write(const T& item) override {
        data.Append(item);
        return (size_t)data.GetLength();
    }
    void Open() override {}
    void Close() override {}

    //забрать накопленный результат
    const MutableArraySequence<T>& GetResult() const { return data; }
};

// запись в текстовый файл: каждый элемент сериализуется в строку и пишется отдельной строкой
template <class T>
class FileWriteStream : public WriteOnlyStream<T> {
private:
    std::string filename;
    std::function<std::string(const T&)> serialize;
    std::ofstream file;
    size_t pos;
public:
    FileWriteStream(const std::string& filename, std::function<std::string(const T&)> serialize)
        : filename(filename), serialize(serialize), pos(0) {}

    void Open() override {
        file.open(filename);
        if (!file.is_open()) // частая причина: папки из пути не существует (ofstream её не создаст)
            throw InvalidArgument(("файл не открылся для записи: " + filename).c_str());
    }
    void Close() override {
        if (file.is_open())
            file.close();
    }
    size_t GetPosition() const override { return pos; }
    size_t Write(const T& item) override {
        file << serialize(item) << "\n";
        pos++;
        return pos;
    }

    ~FileWriteStream() override {
        if (file.is_open())
            file.close();
    }
};

#endif //LAB4_WRITESTREAMS_H
