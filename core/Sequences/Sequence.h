#ifndef LAB2_SEQUENCE_H
#define LAB2_SEQUENCE_H

#include "../../my_except.h"
#include "ICollection.h"

template <class T>

class Sequence: public ICollection<T>{
protected:
    virtual Sequence<T>* Instance() const = 0;
    virtual void AppendInPlace(const T& item) = 0; // чтобы избежать N копий в конкате
public:
    // получаем что-то
    virtual int GetLength() const = 0;
    int GetCount() const override { return GetLength(); }
    virtual T Get(int index) const override = 0;
    T GetFirst() const { return Get(0); }
    T GetLast() const { return Get(GetLength()-1); }
    virtual Sequence<T>* GetSubsequence(int start_index, int end_index) const {
        if (start_index < 0 || start_index > end_index || end_index >= GetLength())
            throw OutOfRange("неверные индексы start_index, end_index");
        Sequence<T>* result = this->Instance();
        for (int i = start_index; i <= end_index; i++)
            result->AppendInPlace(Get(i));
        return result;
    }

    // добавляем, получаем указатель на элемент/последовательность
    virtual Sequence<T>* Append(const T &item) = 0;
    virtual Sequence<T>* Prepend(const T &item) = 0;
    virtual Sequence<T>* InsertAt(int index, const T &item) = 0; 
    virtual Sequence<T>* Concat(Sequence<T>* other) const {
        if (other == nullptr)
            throw InvalidArgument("other");
        Sequence<T>* result = Instance();
        for (int i = 0; i < GetLength(); i++)
            result->AppendInPlace(Get(i));
        for (int i = 0; i < other->GetLength(); i++)
            result->AppendInPlace(other->Get(i));
        return result;
    }

    //функции map-where-reduce
    virtual Sequence<T>* Map(T (*f)(const T &)) const {
        Sequence<T>* result = Instance();
        for (int i = 0; i < GetLength(); i++)
            result->AppendInPlace(f(Get(i)));
        return result;
    }

    virtual Sequence<T>* Where(bool (*f)(const T &)) const {
        Sequence<T>* result = Instance();
        for (int i = 0; i < GetLength(); i++)
            if (f(Get(i)))
                result->AppendInPlace(Get(i));
        return result;
    }
    T Reduce(T (*f)(const T &, const T &), const T & start) const {
        T result = start;
        for (int i=0; i<GetLength(); i++) {
            result = f(result, Get(i));
        }
        return result;
    };

    virtual void RemoveAt(int index) = 0;
    void RemoveLast() { RemoveAt(GetLength()-1); }

    virtual ICollection<T>* Clone() const override {
        Sequence<T> *result = Instance();
        for (int i=0; i<GetLength(); i++) {
            result->AppendInPlace(Get(i));
        }
        return result;
    }

    //перопределение оператора
    T operator[](int index) const { return Get(index); }; //поднять

    //Деструктор
    virtual ~Sequence() = default;
};

#endif //LAB2_SEQUENCE_H
