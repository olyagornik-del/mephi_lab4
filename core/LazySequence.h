#ifndef LAB4_LAZYSEQUENCE_H
#define LAB4_LAZYSEQUENCE_H

#include <functional>
#include <cstddef> // size_t
#include "../my_except.h"
#include "Ordinal.h"
#include "Generator.h"
#include "LazySequenceGenerators.h"
#include "Sequences/Sequence.h"
#include "Sequences/MutableArraySequence.h"

// ленивый список — тонкая обёртка над генератором.
// каждая операция оборачивает текущий генератор в новый (map/concat/insert/...)
template <class T>
class LazySequence {
private:
    Generator<T>* gen; // владеет генератором

    //приватный конструктор из готового генератора (для операций)
    explicit LazySequence(Generator<T>* g): gen(g) {}

    template <class U> friend class LazySequence; // Map<T2> строит список другого типа

public:
    //пустой список
    LazySequence(): gen(new SequenceGenerator<T>()) {}

    //из массива
    LazySequence(T* items, int count): gen(nullptr) {
        if (count < 0)
            throw InvalidArgument("count");
        gen = new SequenceGenerator<T>(items, count);
    }

    //из обычной последовательности
    explicit LazySequence(Sequence<T>* seq): gen(new SequenceGenerator<T>(seq)) {}

    //из рекуррентного правила и начальных элементов — бесконечный список
    LazySequence(std::function<T(Sequence<T>*)> rule, Sequence<T>* seed)
        : gen(new RecurrenceGenerator<T>(rule, seed)) {}

    //копирующий конструктор — клонируем генератор целиком
    LazySequence(const LazySequence<T>& other): gen(other.gen->Clone()) {}
    LazySequence<T>& operator=(const LazySequence<T>&) = delete; //запрет на присваивание

    //получить элемент по ОРДИНАЛЬНОМУ индексу (умеет прыгать через ω)
    T GetByOrdinal(const Ordinal& index) { return gen->Get(index); }

    //получить элемент по обычному (конечному) индексу
    T Get(int index) {
        if (index < 0)
            throw OutOfRange("отрицательный индекс в Get");
        return gen->Get(Ordinal::Finite((unsigned long long)index));
    }

    //получить первый элемент
    T GetFirst() { return Get(0); }

    //получить последний элемент (для бесконечного списка недостижим)
    T GetLast() {
        Ordinal len = gen->GetLength();
        if (len.IsInfinite())
            throw OutOfRange("последний элемент бесконечного списка недостижим");
        if (len.IsZero())
            throw OutOfRange("список пуст");
        return gen->Get(Ordinal::Finite(len.GetFinite() - 1));
    }

    //длина: конечная или ω
    Ordinal GetLength() const { return gen->GetLength(); }

    //сколько элементов уже материализовано (для проверки ленивости)
    size_t GetMaterializedCount() const { return gen->GetMaterializedCount(); }

    bool IsInfinite() const { return gen->GetLength().IsInfinite(); }

    //склеить два списка
    LazySequence<T>* Concat(LazySequence<T>* other) {
        if (other == nullptr)
            throw InvalidArgument("other");
        return new LazySequence<T>(new ConcatGenerator<T>(gen->Clone(), other->gen->Clone()));
    }

    //добавить в конец — склейка с одноэлементным списком
    LazySequence<T>* Append(const T& item) {
        T one[1] = { item };
        Generator<T>* tail = new SequenceGenerator<T>(one, 1);
        return new LazySequence<T>(new ConcatGenerator<T>(gen->Clone(), tail));
    }

    //добавить в начало
    LazySequence<T>* Prepend(const T& item) {
        T one[1] = { item };
        Generator<T>* head = new SequenceGenerator<T>(one, 1);
        return new LazySequence<T>(new ConcatGenerator<T>(head, gen->Clone()));
    }

    //вставить в позицию index
    LazySequence<T>* InsertAt(const T& item, int index) {
        if (index < 0)
            throw OutOfRange("отрицательный индекс в InsertAt");
        Ordinal len = gen->GetLength();
        if (len.IsFinite() && (unsigned long long)index > len.GetFinite())
            throw OutOfRange("index", index, 0, (int)len.GetFinite());
        return new LazySequence<T>(new InsertGenerator<T>(gen->Clone(), index, item));
    }

    //map: новый ленивый список, элемент i = f(исходный[i]) (тип может меняться)
    template <class T2>
    LazySequence<T2>* Map(std::function<T2(const T&)> f) {
        return new LazySequence<T2>(new MapGenerator<T2, T>(gen->Clone(), f));
    }

    //where: новый ленивый список из подходящих элементов
    LazySequence<T>* Where(std::function<bool(const T&)> pred) {
        return new LazySequence<T>(new WhereGenerator<T>(gen->Clone(), pred));
    }

    //reduce: свёртка; для бесконечного списка не определена
    template <class T2>
    T2 Reduce(std::function<T2(const T2&, const T&)> f, const T2& start) {
        if (IsInfinite())
            throw OutOfRange("Reduce не определён для бесконечного списка");
        unsigned long long n = gen->GetLength().GetFinite();
        T2 acc = start;
        for (unsigned long long i = 0; i < n; i++)
            acc = f(acc, gen->Get(Ordinal::Finite(i)));
        return acc;
    }

    //подпоследовательность [start, end]
    LazySequence<T>* GetSubsequence(int start, int end) {
        if (start < 0 || end < start)
            throw OutOfRange("неверные индексы start, end");
        Ordinal len = gen->GetLength();
        if (len.IsFinite() && (unsigned long long)end >= len.GetFinite())
            throw OutOfRange("end за пределами списка");
        return new LazySequence<T>(new SubsequenceGenerator<T>(gen->Clone(), start, end - start + 1));
    }

    ~LazySequence() { delete gen; }
};

#endif //LAB4_LAZYSEQUENCE_H
