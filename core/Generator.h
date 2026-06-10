#ifndef LAB4_GENERATOR_H
#define LAB4_GENERATOR_H

#include <functional>
#include <cstddef> // size_t
#include "../my_except.h"
#include "Ordinal.h"
#include "Sequences/Sequence.h"
#include "Sequences/MutableArraySequence.h"

// базовый генератор: даёт элемент по ОРДИНАЛЬНОМУ индексу и знает свою длину.
// от него наследуются конкретные виды (последовательность, рекуррентность, склейка, map и т.д.)
template <class T>
class Generator {
public:
    Generator() = default;
    //генераторы не копируются (только через Clone) — запрет один раз для всех наследников
    Generator(const Generator<T>&) = delete;
    Generator<T>& operator=(const Generator<T>&) = delete;

    virtual T Get(const Ordinal& index) = 0;
    virtual Ordinal GetLength() const = 0;
    virtual size_t GetMaterializedCount() const = 0; // сколько уже посчитано
    virtual Generator<T>* Clone() const = 0;
    virtual ~Generator() = default;

protected:
    //локальный индекс внутри сегмента = globalIndex минус начало сегмента (при segmentStart <= globalIndex)
    // например в конкате считает номер элемента внутри нужной из сконкаченных последовательностей
    static Ordinal LocalIndex(const Ordinal& globalIndex, const Ordinal& segmentStart) {
        unsigned long long startOmega = segmentStart.GetOmegaCoeff();
        unsigned long long startFin = segmentStart.GetFinite();

        unsigned long long idxOmega = globalIndex.GetOmegaCoeff();
        unsigned long long idxFin = globalIndex.GetFinite();

        if (idxOmega > startOmega) // перешагнули хотя бы один бесконечный сегмент
            return Ordinal::FromParts(idxOmega - startOmega, idxFin);
        return Ordinal::Finite(idxFin - startFin); // тот же сегмент — вычитаем хвосты
    }
};

// конечный список из готовых элементов (массив/последовательность)
template <class T>
class SequenceGenerator : public Generator<T> {
private:
    MutableArraySequence<T> data;
public:
    SequenceGenerator(): data() {}
    SequenceGenerator(T* items, int count): data(items, count) {}
    explicit SequenceGenerator(Sequence<T>* seq): data() {
        if (seq == nullptr)
            throw InvalidArgument("seq");
        for (int i = 0; i < seq->GetLength(); i++)
            data.Append(seq->Get(i));
    }
    explicit SequenceGenerator(const MutableArraySequence<T>& d): data(d) {} // для Clone

    T Get(const Ordinal& index) override {
        if (index.IsInfinite())
            throw OutOfRange("бесконечный индекс недостижим в конечном списке");
        int i = (int)index.GetFinite();
        if (i < 0 || i >= data.GetLength())
            throw OutOfRange("index", i, 0, data.GetLength() - 1);
        return data.Get(i);
    }
    Ordinal GetLength() const override {
        return Ordinal::Finite(data.GetLength());
    }
    size_t GetMaterializedCount() const override {
        return (size_t)data.GetLength();
    }
    Generator<T>* Clone() const override {
        return new SequenceGenerator<T>(data);
    }
};

// бесконечный список по рекуррентному правилу + начальные элементы (seed).
// помнит уже высчитанные элементы , т.к. правилу нужны предыдущие
template <class T>
class RecurrenceGenerator : public Generator<T> {
private:
    std::function<T(Sequence<T>*)> rule;
    MutableArraySequence<T> generated; // seed + уже порождённые

    //домотать до индекса index включительно
    void MaterializeUpTo(int index) {
        while (generated.GetLength() <= index)
            generated.Append(rule(&generated));
    }
public:
    RecurrenceGenerator(std::function<T(Sequence<T>*)> rule, Sequence<T>* seed)
        : rule(rule), generated() {
        if (seed == nullptr)
            throw InvalidArgument("seed");
        for (int i = 0; i < seed->GetLength(); i++)
            generated.Append(seed->Get(i));
    }
    //для Clone — копируем правило и уже посчитанные элементы
    RecurrenceGenerator(std::function<T(Sequence<T>*)> rule, const MutableArraySequence<T>& g)
        : rule(rule), generated(g) {}

    T Get(const Ordinal& index) override {
        if (index.IsInfinite())
            throw OutOfRange("бесконечный индекс недостижим в простом списке");
        int i = (int)index.GetFinite();
        MaterializeUpTo(i);
        return generated.Get(i);
    }
    Ordinal GetLength() const override {
        return Ordinal::Omega();
    }
    size_t GetMaterializedCount() const override {
        return (size_t)generated.GetLength();
    }
    Generator<T>* Clone() const override {
        return new RecurrenceGenerator<T>(rule, generated);
    }
};

#endif //LAB4_GENERATOR_H
