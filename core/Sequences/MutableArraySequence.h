#ifndef LAB2_MUTABLEARRAYSEQUENCE_H
#define LAB2_MUTABLEARRAYSEQUENCE_H

#include "../DynamicArray.h"
#include "Sequence.h"



template <class T>
class MutableArraySequence : public Sequence<T> {
private:
    DynamicArray<T> data;
protected:
    virtual MutableArraySequence<T>* MakeInstance(){ return this; }
    Sequence<T>* Instance() const override { return new MutableArraySequence<T>(); }
    void AppendInPlace(const T& item) override {
        int n = data.GetSize();
        data.Resize(n + 1);
        data.Set(n, item);
    }
public:
    //конструкторы;
    MutableArraySequence(): data() {}
    explicit MutableArraySequence(int size): data(size) {}
    MutableArraySequence(T *items, int size): data(items, size) {}
    explicit MutableArraySequence(const DynamicArray<T> &other_arr) : data(other_arr) {}

    //получаем длину
    int GetLength() const override{
        return data.GetSize();
    }
    // получаем элемент по индексу
    T Get(int index) const override {
        if (GetLength() == 0)
            throw OutOfRange("Последовательность пуста");
        if (index < 0 || index >= GetLength())
            throw OutOfRange("index", index, 0, GetLength() - 1);
        return data.Get(index);
    }

    // добавляем в конец
    Sequence<T>* Append(const T &item) override {
        MutableArraySequence<T> *result = MakeInstance();
        int old_size = GetLength();
        result->data.Resize(old_size + 1);
        result->data.Set(old_size, item);
        return result;
    }
    //добавляем в начало
    Sequence<T>* Prepend(const T &item) override {
        MutableArraySequence<T> *result =MakeInstance();
        int size = GetLength();
        result->data.Resize(size + 1);
        for (int i = size; i > 0; i--)
            result->data.Set(i, Get(i - 1));
        result->data.Set(0, item);
        return result;
    }
    //вставить по индексу
    Sequence<T>* InsertAt(int index, const T &item) override {
        if (index < 0 || index > GetLength()) {
            throw OutOfRange("index", index, 0, GetLength());
        }
        if (index == 0)
            return Prepend(item);
        if (index == GetLength())
            return Append(item);
        MutableArraySequence<T> *result = MakeInstance();
        int size = GetLength();
        result->data.Resize(size + 1);
        for (int i = size; i > index; i--)
            result->data.Set(i, Get(i - 1));
        result->data.Set(index, item);
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

#endif //LAB2_MUTABLEARRAYSEQUENCE_H