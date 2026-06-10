#ifndef LAB4_BINARYHEAP_H
#define LAB4_BINARYHEAP_H

#include <functional> //для class Compare = std::less<T>,
// то есть если нужна куча откуда берём минимум, то просто BinaryHeap<Т> h;
// а если нужна куча откуда быстро берём макс, то BinaryHeap<Т, std::greater<T>> h2;
// и живём счастливо (по возможности)
#include "../core/DynamicArray.h"


//для элемента с индексом i:
//   левый ребёнок = 2i + 1
//   правый ребёнок = 2i + 2
//   родитель = (i - 1) / 2

template <class T, class Compare = std::less<T>>
class BinaryHeap {
private:
    DynamicArray<T> data;
    Compare cmp; //less: cmp(a, b) если a<b - true
    void SwapAt(int from, int to) {
        T tmp = data.Get(from);
        data.Set(from, data.Get(to));
        data.Set(to, tmp);
    }
public:
    void Insert(const T& x) {
        data.Resize(data.GetSize()+1);
        int i = data.GetSize() - 1;
        data.Set(i, x);
        while (i > 0) {
            int parent = (i-1)/2;
            if ( cmp(data.Get(i), data.Get(parent)) ) {
                SwapAt(i, parent);
                i = parent;
            }
            else {
                break;
            }
        } // while
    } // Insert

    T ExtractMinOrMax() {
        if (IsEmpty()) {
            throw OutOfRange("ExtractMin из пустой кучи");
        }
        //запоминаем топ и перекидываем хвост наверх
        auto top = data.Get(0);
        int new_size = data.GetSize() -1;
        data.Set(0, data.Get(new_size));
        data.Resize(new_size);

        int i = 0;
        while (true) {
            int left = 2*i + 1;
            int right = 2*i + 2;
            int best = i;
            if ( (left<new_size) && cmp(data.Get(left), data.Get(best) )) {
                best = left;
            }
            if ( (right<new_size) && cmp(data.Get(right), data.Get(best) )) {
                best = right;
            }
            if (best == i) {
                break;
            }
            SwapAt(i, best);
            i = best;
        }
        return top;
    }
    T Top() const {
        if (IsEmpty()) {
            throw OutOfRange("Top пустой кучи");
        }
        return data.Get(0);
    }
    int Size() const {
        return data.GetSize();
    }
    bool IsEmpty() const {
        return data.GetSize() == 0;
    }
};




#endif //LAB4_BINARYHEAP_H