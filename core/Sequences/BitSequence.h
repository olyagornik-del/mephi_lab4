#ifndef LAB2_BITSEQUENCE_H
#define LAB2_BITSEQUENCE_H

#include "../../my_except.h"
#include "../DynamicArray.h"
#include "Sequence.h"

typedef bool Bit;

class BitSequence: public Sequence<Bit> {
private:
    DynamicArray<unsigned int> blocks;
    int bit_count; // кол-во битов
    static const int bits_per_block = 8*sizeof(unsigned int);

    void SetBit(int index, Bit value) {
        int block_i = index / bits_per_block;
        int bit_i   = index % bits_per_block;
        unsigned int b = blocks.Get(block_i);
        if (value) {
            b |= (1u << bit_i); // поставить 1
        }
        else {
            b &= ~(1u << bit_i); // сбросить в 0
        }
        blocks.Set(block_i, b);
    }
    void GrowIfNeeded() {
        if (bit_count % bits_per_block == 0) //тоже самое что и (bit_count+1)>blocks.GetSize()*bits_per_block
            blocks.Resize(blocks.GetSize() + 1);
    }
protected:
    Sequence<Bit>* Instance() const override { return new BitSequence(); }
    void AppendInPlace(const Bit& item) override { GrowIfNeeded(); bit_count++; SetBit(bit_count-1, item); }
public:
    //конструкторы
    BitSequence(): blocks(), bit_count(0) {}
    explicit BitSequence(int size): blocks(size<0? 0: (size + bits_per_block -1)/bits_per_block),
    bit_count(size<0? 0: size) {
        if (size<0) {
            throw InvalidArgument("size");
        }
    }
    BitSequence (Bit *items, int count): blocks(count<0? 0: (count + bits_per_block -1 )/bits_per_block),
                                                bit_count(count<0? 0: count) {
        if (count<0) {
            throw InvalidArgument("count");
        }
        if (count==0) {
            return;
        }
        for (int i = 0; i < bit_count; i++) {
            SetBit(i, items[i]);
        }
    }

    //получаем что-то
    int GetLength() const override {
        return bit_count;
    }
    Bit Get(int index) const override {
        if (GetLength() == 0) {
            throw OutOfRange("Последовательность пуста");
        }
        if (index < 0 || index >= bit_count) {
            throw OutOfRange("index", index, 0, bit_count - 1);
        }
        int block_i = index / bits_per_block;
        int bit_i = index % bits_per_block;
        unsigned int b = blocks.Get(block_i);
        return ((b>>bit_i)&1u);
    }
    Sequence<Bit>* GetSubsequence(int start_index, int end_index) const override {
        if (start_index < 0 || start_index > end_index || end_index >= GetLength()) {
            throw OutOfRange("неверные индексы start_index, end_index");
        }
        int sub_size = end_index - start_index + 1;
        Bit *temp = new Bit[sub_size];
        for (int i=0; i<sub_size; i++) {
            temp[i] = Get(i+start_index);
        }
        Sequence<Bit> *result = new BitSequence(temp, sub_size);
        delete[] temp;
        return result;
    }

    //добавляем в конец
    Sequence<Bit>* Append(const Bit &item) override {
        GrowIfNeeded();
        bit_count++;
        SetBit(bit_count - 1, item);
        return this;
    }
    //добавляем в начало
    Sequence<Bit>* Prepend(const Bit &item) override {
        GrowIfNeeded();
        bit_count++;
        for (int i = bit_count - 1; i >= 1; i--)
            SetBit(i, Get(i - 1));
        SetBit(0, item);
        return this;
    }
    //вставить по индексу
    Sequence<Bit>* InsertAt(int index, const Bit &item) override {
        if (index < 0 || index > GetLength()) {
            throw OutOfRange("index", index, 0, GetLength());
        }
        if (index == 0)
            return Prepend(item);
        if (index == GetLength())
            return Append(item);
        int old_size = bit_count;
        GrowIfNeeded();
        bit_count++;
        for (int i=old_size; i>index; i--) {
            SetBit(i, Get(i-1));
        }
        SetBit(index, item);
        return this;
    }
    //склейка
    Sequence<Bit>* Concat(Sequence<Bit> *other) const override {
        int new_size = GetLength() + other->GetLength();
        BitSequence *result = new BitSequence(new_size);
        for (int i=0; i<GetLength(); i++) {
            result->SetBit(i, Get(i));
        }
        for (int i=0; i<other->GetLength();i++) {
            result->SetBit(i+GetLength(), other->Get(i));
        }
        return result;
    }

    void RemoveAt(int index) override {
        if (GetLength() == 0) {
            throw OutOfRange("Последовательность пуста");
        }
        if (index < 0 || index > GetLength()) {
            throw OutOfRange("index", index, 0, GetLength());
        }
        for (int i = index; i < bit_count - 1; i++) {
            SetBit(i, Get(i + 1));
        }
        bit_count--;
    }

    //побитовые операции
    BitSequence* And(const BitSequence *other) const {
        if (other==nullptr) {
            throw InvalidArgument("other (пустой)");
        }
        if (bit_count != other->bit_count) {
            throw InvalidArgument("несовпадение длин");
        }
        BitSequence *result = new BitSequence(bit_count);
        for (int i=0; i<blocks.GetSize();i++) {
            result->blocks.Set(i, this->blocks.Get(i) & other->blocks.Get(i));
        }
        return result;
    }
    BitSequence* Or(const BitSequence *other) const {
        if (other==nullptr) {
            throw InvalidArgument("other");
        }
        if (bit_count != other->bit_count) {
            throw InvalidArgument("несовпадение длин"
                );
        }
        BitSequence *result = new BitSequence(bit_count);
        for (int i=0; i<blocks.GetSize();i++) {
            result->blocks.Set(i, this->blocks.Get(i) | other->blocks.Get(i));
        }
        return result;
    }
    BitSequence* Xor(const BitSequence *other) const {
        if (other==nullptr) {
            throw InvalidArgument("other");
        }
        if (bit_count != other->bit_count) {
            throw InvalidArgument("несовпадение длин");
        }
        BitSequence *result = new BitSequence(bit_count);
        for (int i=0; i<blocks.GetSize();i++) {
            result->blocks.Set(i, this->blocks.Get(i) ^ other->blocks.Get(i));
        }
        return result;
    }
    BitSequence* Not() const {
        BitSequence *result = new BitSequence(bit_count);
        for (int i = 0; i < blocks.GetSize(); i++)
            result->blocks.Set(i, ~blocks.Get(i));
        // вычистить хвост последнего блока
        int tail_bits = bit_count % bits_per_block;
        if (tail_bits != 0 && blocks.GetSize() > 0) {
            int last = blocks.GetSize() - 1;
            unsigned int mask = (1u << tail_bits) - 1;
            result->blocks.Set(last, result->blocks.Get(last) & mask);
        }
        return result;
    }
};

#endif //LAB2_BITSEQUENCE_H