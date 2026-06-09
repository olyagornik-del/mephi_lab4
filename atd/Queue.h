#ifndef LAB3_QUEUE_H
#define LAB3_QUEUE_H

#include "../my_except.h"
#include "../core/Sequences/MutableListSequence.h"
#include "../core/Sequences/ICollection.h"


template <class T>
class Queue : public ICollection<T> {
private:
    MutableListSequence<T> data;

public:
    Queue() = default;
    Queue<T>& operator=(const Queue<T>&) = delete;

    void Enqueue(const T& item) { data.Append(item); }
    T Dequeue() {
        if (IsEmpty())
            throw OutOfRange("Dequeue из пустой очереди");
        T front = data.GetFirst();
        data.RemoveAt(0);
        return front;
    }
    T Peek() const { return data.GetFirst(); }
    bool IsEmpty() const { return data.GetLength() == 0; }
    int  Size() const { return data.GetLength(); }


    Queue<T>* Map(T (*f)(const T &)) const {
        Queue<T>* result = new Queue<T>();
        for (auto it = data.GetIterator(); it.HasNext(); it.Next())
            result->data.Append(f(it.Current()));
        return result;
    }
    Queue<T>* Where(bool (*f)(const T &)) const {
        Queue<T>* result = new Queue<T>();
        for (auto it = data.GetIterator(); it.HasNext(); it.Next())
            if (f(it.Current()))
                result->data.Append(it.Current());
        return result;
    }
    T Reduce(T (*f)(const T &, const T &), const T & start) const {
        T acc = start;
        for (auto it = data.GetIterator(); it.HasNext(); it.Next())
            acc = f(acc, it.Current());
        return acc;
    }
    Queue<T>* Concat(const Queue<T>* other) const {
        if (other == nullptr)
            throw InvalidArgument("other");
        Queue<T>* result = new Queue<T>(*this);
        for (auto it = other->data.GetIterator(); it.HasNext(); it.Next())
            result->data.Append(it.Current());
        return result;
    }
    Queue<T>* GetSubsequence(int from, int to) const {
        if (from < 0 || from > to || to >= Size())
            throw OutOfRange("неверные индексы from, to");
        Queue<T>* result = new Queue<T>();
        int i = 0;
        for (auto it = data.GetIterator(); it.HasNext(); it.Next(), i++) {
            if (i < from) continue;
            if (i > to) break;
            result->data.Append(it.Current());
        }
        return result;
    }
    int FindSubsequence(const Queue<T>* sub) const { // наивный поиск, индекс первого вхождения
        if (sub == nullptr)
            throw InvalidArgument("sub");
        int this_size = Size();
        int sub_size = sub->Size();
        if (sub_size == 0) { throw InvalidArgument("sub"); }
        if (sub_size > this_size) { return -1; }
        for (int i = 0; i + sub_size <= this_size; i++) {
            bool match = true;
            for (int j = 0; j < sub_size; j++) {
                if (!(Get(i + j) == sub->Get(j))) { match = false; break; }
            }
            if (match) { return i; }
        }
        return -1; // не найдено
    }

    // из ICollection:
    T Get(int index) const override { return data.Get(index); }
    int GetCount() const override { return Size(); }
    ICollection<T>* Clone() const override { return new Queue<T>(*this); }

    //операторы
    bool operator==(const Queue<T>& other) const {
        if (Size() != other.Size())
            return false;
        auto a = data.GetIterator();
        auto b = other.data.GetIterator();
        while (a.HasNext()) {
            if (!(a.Current() == b.Current()))
                return false;
            a.Next(); b.Next();
        }
        return true;
    }
    bool operator!=(const Queue<T>& other) const {
        return !(*this == other);
    }
};

#endif //LAB3_QUEUE_H
