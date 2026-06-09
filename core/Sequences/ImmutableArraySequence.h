#ifndef LAB2_IMMUTABLEARRAYSEQUENCE_H
#define LAB2_IMMUTABLEARRAYSEQUENCE_H

#include "MutableArraySequence.h"

template <class T>
class ImmutableArraySequence : public MutableArraySequence<T> {
protected:
    MutableArraySequence<T>* MakeInstance() override {
        return new ImmutableArraySequence<T>(*this); // копия через конструктор копирования
    }
    Sequence<T>* Instance() const override { return new ImmutableArraySequence<T>(); }
public:
    ImmutableArraySequence() : MutableArraySequence<T>() {}
    explicit ImmutableArraySequence(int size) : MutableArraySequence<T>(size) {}
    ImmutableArraySequence(T *items, int size) : MutableArraySequence<T>(items, size) {}
};

#endif //LAB2_IMMUTABLEARRAYSEQUENCE_H