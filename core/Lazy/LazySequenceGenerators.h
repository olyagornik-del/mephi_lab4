#ifndef LAB4_LAZYSEQUENCEGENERATORS_H
#define LAB4_LAZYSEQUENCEGENERATORS_H

#include <functional>
#include "Generator.h"

// склейка двух генераторов: сначала весь left, потом right
template <class T>
class ConcatGenerator : public Generator<T> {
private:
    Generator<T>* left;
    Generator<T>* right;
public:
    ConcatGenerator(Generator<T>* left, Generator<T>* right): left(left), right(right) {}

    T Get(const Ordinal& index) override {
        Ordinal leftLen = left->GetLength();
        if (index < leftLen)
            return left->Get(index); // в первом сегменте
        return right->Get(Generator<T>::LocalIndex(index, leftLen)); // прыжок во второй
    }
    Ordinal GetLength() const override {
        return left->GetLength() + right->GetLength();
    }
    size_t GetMaterializedCount() const override {
        return left->GetMaterializedCount() + right->GetMaterializedCount();
    }
    Generator<T>* Clone() const override {
        return new ConcatGenerator<T>(left->Clone(), right->Clone());
    }

    ~ConcatGenerator() override { delete left; delete right; }
};

// отложенная вставка одного элемента в позицию pos
template <class T>
class InsertGenerator : public Generator<T> {
private:
    Generator<T>* base;
    int pos;
    T* value;
public:
    InsertGenerator(Generator<T>* base, int pos, const T& value)
        : base(base), pos(pos), value(new T(value)) {}

    T Get(const Ordinal& index) override {
        if (index.IsFinite()) {
            unsigned long long n = index.GetFinite();
            if (n < (unsigned long long)pos)
                return base->Get(index); // до вставки
            if (n == (unsigned long long)pos)
                return *value; // сама вставка
            return base->Get(Ordinal::Finite(n - 1)); // после — сдвиг на 1
        }
        return base->Get(index); // бесконечный индекс конечная вставка не сдвигает
    }
    Ordinal GetLength() const override {
        Ordinal baseLen = base->GetLength();
        if (baseLen.IsInfinite()) // вставка в бесконечный список длину не меняет
            return baseLen;
        return baseLen + Ordinal::Finite(1);
    }
    size_t GetMaterializedCount() const override {
        return base->GetMaterializedCount();
    }
    Generator<T>* Clone() const override {
        return new InsertGenerator<T>(base->Clone(), pos, *value);
    }

    ~InsertGenerator() override { delete base; delete value; }
};

// map: элемент i = f(source[i]); тип может меняться (TSource -> TResult)
template <class TResult, class TSource>
class MapGenerator : public Generator<TResult> {
private:
    Generator<TSource>* source;
    std::function<TResult(const TSource&)> f;
public:
    MapGenerator(Generator<TSource>* source, std::function<TResult(const TSource&)> f)
        : source(source), f(f) {}

    TResult Get(const Ordinal& index) override {
        return f(source->Get(index));
    }
    Ordinal GetLength() const override {
        return source->GetLength();
    }
    size_t GetMaterializedCount() const override {
        return source->GetMaterializedCount();
    }
    Generator<TResult>* Clone() const override {
        return new MapGenerator<TResult, TSource>(source->Clone(), f);
    }

    ~MapGenerator() override { delete source; }
};

// where: подходящие под предикат элементы
template <class T>
class WhereGenerator : public Generator<T> {
private:
    Generator<T>* source;
    std::function<bool(const T&)> pred;
    Ordinal len; // длина результата
public:
    WhereGenerator(Generator<T>* source, std::function<bool(const T&)> pred)
        : source(source), pred(pred), len() {
        Ordinal srcLen = source->GetLength();
        if (srcLen.IsInfinite()) {
            len = Ordinal::Omega(); // допускаем бесконечно много подходящих
        } else {
            unsigned long long cnt = 0;
            unsigned long long n = srcLen.GetFinite();
            for (unsigned long long i = 0; i < n; i++)
                if (pred(source->Get(Ordinal::Finite(i))))
                    cnt++;
            len = Ordinal::Finite(cnt);
        }
    }
    //для Clone — длина уже известна
    WhereGenerator(Generator<T>* source, std::function<bool(const T&)> pred, const Ordinal& len)
        : source(source), pred(pred), len(len) {}

    T Get(const Ordinal& index) override {
        unsigned long long target = index.GetFinite(); // нужен target-й подходящий
        Ordinal srcLen = source->GetLength();
        unsigned long long n = srcLen.IsInfinite() ? 0 : srcLen.GetFinite();
        unsigned long long matched = 0;
        unsigned long long i = 0;
        while (srcLen.IsInfinite() || i < n) {
            T v = source->Get(Ordinal::Finite(i));
            if (pred(v)) {
                if (matched == target)
                    return v; // не знаю как обойти если таргет никогда не достижим (tagret = 11 x<10 x = 1, 2, 3,...)
                matched++;
            }
            i++;
        }
        throw OutOfRange("индекс за пределами отфильтрованного списка");
    }
    Ordinal GetLength() const override {
        return len;
    }
    size_t GetMaterializedCount() const override {
        return source->GetMaterializedCount();
    }
    Generator<T>* Clone() const override {
        return new WhereGenerator<T>(source->Clone(), pred, len);
    }

    ~WhereGenerator() override { delete source; }
};

// подпоследовательность [start, start+count) — элемент i = source[start+i]
template <class T>
class SubsequenceGenerator : public Generator<T> {
private:
    Generator<T>* source;
    int start;
    int count;
public:
    SubsequenceGenerator(Generator<T>* source, int start, int count)
        : source(source), start(start), count(count) {}

    T Get(const Ordinal& index) override {
        if (index.IsInfinite())
            throw OutOfRange("бесконечный индекс недостижим в подпоследовательности");
        int i = (int)index.GetFinite();
        if (i < 0 || i >= count)
            throw OutOfRange("index", i, 0, count - 1);
        return source->Get(Ordinal::Finite((unsigned long long)(start + i)));
    }
    Ordinal GetLength() const override {
        return Ordinal::Finite((unsigned long long)count);
    }
    size_t GetMaterializedCount() const override {
        return source->GetMaterializedCount();
    }
    Generator<T>* Clone() const override {
        return new SubsequenceGenerator<T>(source->Clone(), start, count);
    }

    ~SubsequenceGenerator() override { delete source; }
};

#endif //LAB4_LAZYSEQUENCEGENERATORS_H
