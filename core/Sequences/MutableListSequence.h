
#ifndef LAB2_MUTABLELISTSEQUENCE_H
#define LAB2_MUTABLELISTSEQUENCE_H

#include "../LinkedList.h"
#include "Sequence.h"

template <class T>
class MutableListSequence : public Sequence<T> {
private:
    LinkedList<T> data;
protected:
    virtual MutableListSequence<T>* MakeInstance() { return this; }
    MutableListSequence<T>* Instance() const override { return new MutableListSequence<T>(); }
    void AppendInPlace(const T& item) override { data.Append(item); }
public:
    //конструкторы
    MutableListSequence(): data() {}
    MutableListSequence(T *items, int size ): data(items, size) {}
    explicit MutableListSequence (const LinkedList<T> &other_list): data(other_list) {}

    //получаем длину
    int GetLength() const override {
        return data.GetLength();
    }
    // получаем элемент по индексу
    T Get(int index) const override {
        if (GetLength() == 0)
            throw OutOfRange("Последовательность пуста");
        if (index < 0 || index >= GetLength())
            throw OutOfRange("index", index, 0, GetLength() - 1);
        return data.Get(index);
    }

    T GetFirst() const { return data.GetFirst(); }
    T GetLast()  const { return data.GetLast(); }
    typename LinkedList<T>::Iterator GetIterator() const { return data.GetIterator(); }
    // получаем подпоследовательность
    Sequence<T>* GetSubsequence(int start_index, int end_index) const override {
        if (start_index < 0 || start_index > end_index || end_index >= GetLength())
            throw OutOfRange("неверные индексы start_index, end_index");
        MutableListSequence<T>* result = this->Instance();
        int i = 0;
        for (auto it = data.GetIterator(); it.HasNext(); it.Next(), i++) {
            if (i < start_index) continue;
            if (i > end_index) break;
            result->AppendInPlace(it.Current());
        }
        return result;
    }

    //добавляем в конец
    Sequence<T>* Append(const T &item) override {
        MutableListSequence<T> *result = MakeInstance();
        result->data.Append(item);
        return result;
    }
    //добавляем в начало
    Sequence<T>* Prepend(const T &item) override {
        MutableListSequence<T> *result = MakeInstance();
        result->data.Prepend(item);
        return result;
    }
    //вставить по индексу
    Sequence<T>* InsertAt(int index, const T &item) override {
        if (index < 0 || index > GetLength()) {
            throw OutOfRange("index", index, 0, GetLength());
        }
        MutableListSequence<T> *result = MakeInstance();
        result->data.InsertAt(index, item);
        return result;
    }
    Sequence<T>* Concat(Sequence<T>* other) const override {
        if (other == nullptr)
            throw InvalidArgument("other");
        MutableListSequence<T>* result = this->Instance();
        for (auto it = data.GetIterator(); it.HasNext(); it.Next()) // this — O(n)
            result->AppendInPlace(it.Current());
        for (int i = 0; i < other->GetLength(); i++) // other — O(m^2) если список (да, вот такой компромисс)
            result->AppendInPlace(other->Get(i));
        return result;
    }

    Sequence<T>* Map(T (*f)(const T&)) const override {
        MutableListSequence<T>* result = this->Instance();
        for (auto it = data.GetIterator(); it.HasNext(); it.Next())
            result->AppendInPlace(f(it.Current()));
        return result;
    }

    Sequence<T>* Where(bool (*f)(const T&)) const override {
        MutableListSequence<T>* result = this->Instance();
        for (auto it = data.GetIterator(); it.HasNext(); it.Next())
            if (f(it.Current()))
                result->AppendInPlace(it.Current());
        return result;
    }

    ICollection<T>* Clone() const override {
        MutableListSequence<T>* result = this->Instance();
        for (auto it = data.GetIterator(); it.HasNext(); it.Next()) {
            result->AppendInPlace(it.Current());
        }
        return result;
    }

    void RemoveAt(int index) override {
        if (GetLength() == 0) {
            throw OutOfRange("ну нельзя у пустого массива что-то удалить");
        }
        if (index < 0 || index >= GetLength()) {
            throw OutOfRange("index", index, 0, GetLength() - 1);
        }
        data.RemoveAt(index);
    }
};

#endif //LAB2_MUTABLELISTSEQUENCE_H