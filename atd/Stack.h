#ifndef LAB3_STACK_H
#define LAB3_STACK_H

#include "../my_except.h"
#include "../core/Sequences/MutableListSequence.h"
#include "../core/Sequences/ICollection.h"

template <class T>
class Stack : public ICollection<T> {
private:
    MutableListSequence<T> data;

public:
    Stack() = default; //дефолт конструктор
    Stack<T>& operator=(const Stack<T>&) = delete; // запрет на =

    void Push(const T& item) { data.Append(item); }
    T Peek() const { return data.GetLast(); }
    T Pop() {
        if (IsEmpty())
            throw OutOfRange("Pop из пустого стека");
        T top = Peek();
        data.RemoveLast();
        return top;
    }
    bool IsEmpty() const { return data.GetLength() == 0; }
    int  Size() const { return data.GetLength(); }

    //один проход итератором
    Stack<T>* Map(T (*f)(const T &)) const {
        Stack<T>* result = new Stack<T>();
        for (auto it = data.GetIterator(); it.HasNext(); it.Next())
            result->data.Append(f(it.Current()));
        return result;
    }
    Stack<T>* Where(bool (*f)(const T &)) const {
        Stack<T>* result = new Stack<T>();
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
    Stack<T>* Concat(const Stack<T>* other) const {
        if (other == nullptr)
            throw InvalidArgument("other");
        Stack<T>* result = new Stack<T>(*this);
        for (auto it = other->data.GetIterator(); it.HasNext(); it.Next())
            result->data.Append(it.Current());
        return result;
    }
    Stack<T>* GetSubsequence(int from, int to) const {
        if (from < 0 || from > to || to >= Size())
            throw OutOfRange("неверные индексы from, to");
        Stack<T>* result = new Stack<T>();
        int i = 0;
        for (auto it = data.GetIterator(); it.HasNext(); it.Next(), i++) {
            if (i < from) continue;
            if (i > to) break;
            result->data.Append(it.Current());
        }
        return result;
    }
    int FindSubsequence(const Stack<T>* sub) const {
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
    ICollection<T>* Clone() const override { return new Stack<T>(*this); }

    //операторы
    bool operator==(const Stack<T>& other) const {
        if (Size() != other.Size())
            return false;
        auto a = data.GetIterator();
        auto b = other.data.GetIterator();
        while (a.HasNext()) { // параллельный проход O(n)
            if (!(a.Current() == b.Current()))
                return false;
            a.Next();
            b.Next();
        }
        return true;
    }
    bool operator!=(const Stack<T>& other) const {
        return !(*this == other);
    }
};

#endif //LAB3_STACK_H
