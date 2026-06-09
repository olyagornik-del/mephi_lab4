#include <cassert> // assert
#include <cstdio> //printf

#include "../core/Sequences/MutableArraySequence.h"
#include "../core/Sequences/MutableListSequence.h"

void testMutableArraySequenceAppend() {
    MutableArraySequence<int> Seq;
    Sequence<int>* result = Seq.Append(1);
    assert(result == &Seq);
    assert(Seq.GetLength() == 1);

    Seq.Append(2);
    Seq.Append(3);
    assert(Seq.GetLength() == 3);
    assert(Seq.GetFirst() == 1);
    assert(Seq.GetLast() == 3);

    printf("  [OK] testMutableArraySequenceAppend\n");
}

void testMutableArraySequencePrepend() {
    MutableArraySequence<int> Seq;
    Sequence<int>* result = Seq.Prepend(3);
    assert(result == &Seq);
    assert(Seq.GetLength() == 1);

    Seq.Prepend(2);
    Seq.Prepend(1);
    assert(Seq.GetLength() == 3);
    assert(Seq.GetFirst() == 1);
    assert(Seq.GetLast() == 3);

    printf("  [OK] testMutableArraySequencePrepend\n");
}

void testMutableArraySequenceInsertAt() {
    int items[] = {1, 3};
    MutableArraySequence<int> Seq(items, 2);

    Sequence<int>* result = Seq.InsertAt(1, 2);
    assert(result == &Seq);
    assert(Seq.GetLength() == 3);
    assert(Seq.Get(1) == 2);

    Seq.InsertAt(0, 0);
    assert(Seq.GetFirst() == 0);

    Seq.InsertAt(Seq.GetLength(), 99);
    assert(Seq.GetLast() == 99);

    bool threw = false;
    try { Seq.InsertAt(-1, 0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testMutableArraySequenceInsertAt\n");
}

void testMutableListSequenceAppend() {
    MutableListSequence<int> Seq;
    Sequence<int>* result = Seq.Append(1);
    assert(result == &Seq);
    assert(Seq.GetLength() == 1);

    Seq.Append(2);
    Seq.Append(3);
    assert(Seq.GetLength() == 3);
    assert(Seq.GetFirst() == 1);
    assert(Seq.GetLast() == 3);

    printf("  [OK] testMutableListSequenceAppend\n");
}

void testMutableListSequencePrepend() {
    MutableListSequence<int> Seq;
    Sequence<int>* result = Seq.Prepend(3);
    assert(result == &Seq);
    assert(Seq.GetLength() == 1);

    Seq.Prepend(2);
    Seq.Prepend(1);
    assert(Seq.GetLength() == 3);
    assert(Seq.GetFirst() == 1);
    assert(Seq.GetLast() == 3);

    printf("  [OK] testMutableListSequencePrepend\n");
}

void testMutableListSequenceInsertAt() {
    int items[] = {1, 3};
    MutableListSequence<int> Seq(items, 2);

    Sequence<int>* result = Seq.InsertAt(1, 2);
    assert(result == &Seq);
    assert(Seq.GetLength() == 3);
    assert(Seq.Get(1) == 2);

    Seq.InsertAt(0, 0);
    assert(Seq.GetFirst() == 0);

    Seq.InsertAt(Seq.GetLength(), 99);
    assert(Seq.GetLast() == 99);

    bool threw = false;
    try { Seq.InsertAt(-1, 0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testMutableListSequenceInsertAt\n");
}

void testMutableAll() {
    printf("=== Тесты Mutable (Array + List) ===\n");
    testMutableArraySequenceAppend();
    testMutableArraySequencePrepend();
    testMutableArraySequenceInsertAt();
    testMutableListSequenceAppend();
    testMutableListSequencePrepend();
    testMutableListSequenceInsertAt();
    printf("=== Все тесты пройдены! ===\n\n");
}
