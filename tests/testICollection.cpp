#include <cassert> // assert
#include <cstdio> //printf

#include "../core/Sequences/MutableArraySequence.h"
#include "../core/Sequences/MutableListSequence.h"
#include "../core/Sequences/ICollection.h"

void testICollectionArrayAccess() {
    // полиморфный доступ к array-бэкенду через ICollection<T>*
    int items[] = {10, 20, 30};
    MutableArraySequence<int> Seq(items, 3);
    ICollection<int>* col = &Seq;

    assert(col->GetCount() == 3);          // обёртка над GetLength
    assert(col->Get(0) == 10);
    assert(col->Get(2) == 30);

    bool threw = false;
    try { col->Get(3); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testICollectionArrayAccess\n");
}

void testICollectionListAccess() {
    // тот же контракт, но list-бэкенд
    int items[] = {10, 20, 30};
    MutableListSequence<int> Seq(items, 3);
    ICollection<int>* col = &Seq;

    assert(col->GetCount() == 3);
    assert(col->Get(0) == 10);
    assert(col->Get(2) == 30);

    bool threw = false;
    try { col->Get(3); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testICollectionListAccess\n");
}

void testICollectionArrayClone() {
    int items[] = {1, 2, 3};
    MutableArraySequence<int> Seq(items, 3);
    ICollection<int>* col = &Seq;

    ICollection<int>* Copy = col->Clone();
    assert(Copy->GetCount() == 3);
    assert(Copy->Get(0) == 1);
    assert(Copy->Get(2) == 3);

    // копия независима: меняем оригинал — копия не меняется
    Seq.Append(99);
    assert(Seq.GetLength() == 4);
    assert(Copy->GetCount() == 3);         // 99 не просочилась в копию

    delete Copy;                           // удаление через ICollection<T>* — проверка виртуального деструктора

    printf("  [OK] testICollectionArrayClone\n");
}

void testICollectionListClone() {
    int items[] = {1, 2, 3};
    MutableListSequence<int> Seq(items, 3);
    ICollection<int>* col = &Seq;

    ICollection<int>* Copy = col->Clone();
    assert(Copy->GetCount() == 3);
    assert(Copy->Get(0) == 1);
    assert(Copy->Get(2) == 3);

    Seq.Append(99);
    assert(Seq.GetLength() == 4);
    assert(Copy->GetCount() == 3);

    delete Copy;

    printf("  [OK] testICollectionListClone\n");
}

void testICollectionAll() {
    printf("=== Тесты ICollection ===\n");
    testICollectionArrayAccess();
    testICollectionListAccess();
    testICollectionArrayClone();
    testICollectionListClone();
    printf("=== Все тесты пройдены! ===\n\n");
}