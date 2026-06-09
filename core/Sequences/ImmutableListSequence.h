#ifndef LAB2_IMMUTABLELISTSEQUENCE_H
#define LAB2_IMMUTABLELISTSEQUENCE_H

#include "MutableListSequence.h"

template <class T>
class ImmutableListSequence : public MutableListSequence<T> {
protected:
    ImmutableListSequence<T>* MakeInstance() override {
        return new ImmutableListSequence<T>(*this);
    }
    ImmutableListSequence<T>* Instance() const override { return new ImmutableListSequence<T>(); }
public:
    ImmutableListSequence(): MutableListSequence<T>() {}
    ImmutableListSequence(T *items, int size): MutableListSequence<T>(items, size) {}
};
#endif //LAB2_IMMUTABLELISTSEQUENCE_H