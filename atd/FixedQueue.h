#ifndef LAB4_FIXEDQUEUE_H
#define LAB4_FIXEDQUEUE_H

#include "Queue.h"
#include "../my_except.h"

template <class T>
class FixedQueue {
private:
    Queue<T> data;
    int capacity;
public:
    explicit FixedQueue(int k):capacity(k) {
        if (k <= 0)
            throw InvalidArgument("k");
    }

    void Push(const T& item) {
        data.Enqueue(item);
        while (data.Size() > capacity)
            data.Dequeue();
    }

    T Get(int index) const {
        if (index >= data.GetCount() || index < 0) {
            throw OutOfRange("index", index, 0, data.GetCount()-1);
        }
        return data.Get(index);
    }
    int Size() const {
        return data.GetCount();
    }
    bool IsFull() const {
        return capacity == data.GetCount();
    }
};
#endif //LAB4_FIXEDQUEUE_H