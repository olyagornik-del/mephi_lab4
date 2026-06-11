#ifndef LAB4_ONLINESTATS_H
#define LAB4_ONLINESTATS_H

#include <functional> // std::greater/std::less для двух куч
#include <cstddef> // size_t
#include "../my_except.h"
#include "../atd/BinaryHeap.h"
#include "../core/Streams/ParentsStream.h"

// вариант 4: сбор статистики потока в режиме онлайн.
// медиана — через две кучи: каждая держит на вершине свой элемент, ближайший к центру.
// min/max/среднее — за O(1) на элемент; медиана — O(log n) на элемент.
// поток можно НЕ хранить целиком: элементы приходят по одному через Add
template <class T>
class OnlineStats {
private:
    BinaryHeap<T, std::greater<T>> lowerHalf; // max-куча: вершина = наибольшее из меньшей половины
    BinaryHeap<T, std::less<T>> upperHalf;    // min-куча: вершина = наименьшее из большей половины
    size_t count;
    T minValue;
    T maxValue;
    double sum; // для бегущего среднего

public:
    OnlineStats(): lowerHalf(), upperHalf(), count(0), minValue(), maxValue(), sum(0) {}

    //принять очередной элемент потока
    void Add(const T& x) {
        // min/max/sum — O(1)
        if (count == 0) {
            minValue = x;
            maxValue = x;
        }
        else {
            if (x < minValue) minValue = x;
            if (maxValue < x) maxValue = x;
        }
        sum += (double)x;
        count++;

        // медиана: кладём в нижнюю половину, если не больше её вершины, иначе в верхнюю
        if (lowerHalf.IsEmpty() || !(lowerHalf.Top() < x))
            lowerHalf.Insert(x);
        else
            upperHalf.Insert(x);

        // балансировка: размеры половин отличаются не больше чем на 1
        if (lowerHalf.Size() > upperHalf.Size() + 1)
            upperHalf.Insert(lowerHalf.ExtractMinOrMax());
        else if (upperHalf.Size() > lowerHalf.Size() + 1)
            lowerHalf.Insert(upperHalf.ExtractMinOrMax());
    }

    //прогнать конечный поток целиком; бесконечный не кончится — сразу исключение
    void ConsumeAll(ReadOnlyStream<T>& stream) {
        if (!stream.IsFinite())
            throw InvalidArgument("бесконечный поток нельзя прочитать целиком — используй ConsumeFirstN");
        while (!stream.IsEndOfStream())
            Add(stream.Read());
    }

    //прочитать первые n элементов — подходит для любых потоков
    void ConsumeFirstN(ReadOnlyStream<T>& stream, size_t n) {
        for (size_t i = 0; i < n && !stream.IsEndOfStream(); i++)
            Add(stream.Read());
    }

    size_t GetCount() const {
        return count;
    }
    T GetMin() const {
        if (count == 0)
            throw OutOfRange("статистика пуста");
        return minValue;
    }
    T GetMax() const {
        if (count == 0)
            throw OutOfRange("статистика пуста");
        return maxValue;
    }
    double GetMean() const {
        if (count == 0)
            throw OutOfRange("статистика пуста");
        return sum / (double)count;
    }
    //текущая медиана: вершина большей половины, при равных — среднее двух вершин
    double GetMedian() const {
        if (count == 0)
            throw OutOfRange("статистика пуста");
        if (lowerHalf.Size() == upperHalf.Size())
            return ((double)lowerHalf.Top() + (double)upperHalf.Top()) / 2.0;
        if (lowerHalf.Size() > upperHalf.Size())
            return (double)lowerHalf.Top();
        return (double)upperHalf.Top();
    }
};

#endif //LAB4_ONLINESTATS_H
