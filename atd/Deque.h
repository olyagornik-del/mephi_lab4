#ifndef LAB3_DEQUE_H
#define LAB3_DEQUE_H

#include "../my_except.h"
#include "../core/Sequences/MutableListSequence.h"
#include "../core/Sequences/ICollection.h"


template <class T>
class Deque : public ICollection<T> {
private:
    MutableListSequence<T> data;

public:
    Deque() = default;
    Deque<T>& operator=(const Deque<T>&) = delete;

    void PushFront(const T& item) { data.Prepend(item); }
    void PushBack(const T& item)  { data.Append(item); }
    T PopFront() {
        if (IsEmpty())
            throw OutOfRange("PopFront из пустого дека");
        T front = data.GetFirst();
        data.RemoveAt(0);
        return front;
    }
    T PopBack() { // O(n): хвост односвязного списка
        if (IsEmpty())
            throw OutOfRange("PopBack из пустого дека");
        T back = data.GetLast();
        data.RemoveAt(Size() - 1);
        return back;
    }
    T PeekFront() const { return data.GetFirst(); }
    T PeekBack()  const { return data.GetLast(); }
    bool IsEmpty() const { return data.GetLength() == 0; }
    int  Size() const { return data.GetLength(); }

    // bulk-операции: один проход итератором O(n)
    Deque<T>* Map(T (*f)(const T &)) const {
        Deque<T>* result = new Deque<T>();
        for (auto it = data.GetIterator(); it.HasNext(); it.Next())
            result->data.Append(f(it.Current()));
        return result;
    }
    Deque<T>* Where(bool (*f)(const T &)) const {
        Deque<T>* result = new Deque<T>();
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
    Deque<T>* Concat(const Deque<T>* other) const {
        if (other == nullptr)
            throw InvalidArgument("other");
        Deque<T>* result = new Deque<T>(*this);
        for (auto it = other->data.GetIterator(); it.HasNext(); it.Next())
            result->data.Append(it.Current());
        return result;
    }
    Deque<T>* GetSubsequence(int from, int to) const {
        if (from < 0 || from > to || to >= Size())
            throw OutOfRange("неверные индексы from, to");
        Deque<T>* result = new Deque<T>();
        int i = 0;
        for (auto it = data.GetIterator(); it.HasNext(); it.Next(), i++) {
            if (i < from) continue;
            if (i > to) break;
            result->data.Append(it.Current());
        }
        return result;
    }
    int FindSubsequence(const Deque<T>* sub) const { // наивный поиск, индекс первого вхождения
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
    ICollection<T>* Clone() const override { return new Deque<T>(*this); }

    //операторы
    bool operator==(const Deque<T>& other) const {
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
    bool operator!=(const Deque<T>& other) const {
        return !(*this == other);
    }
};

#endif //LAB3_DEQUE_H
