#include <cassert> // assert
#include <cstdio> //printf

#include "../core/Sequences/MutableArraySequence.h"

//testArraySequence вспомогательные функции для Map / Where / Reduce
static int DoubleIt(const int& x) {
    return x * 2;
}
static bool IsEven(const int& x) {
    return x % 2 == 0;
}
static int AddInts(const int& a, const int& b) {
    return a + b;
}

void testArraySequenceCreate() {
    // пустая последовательность
    MutableArraySequence<int> Empty;
    assert(Empty.GetLength() == 0);

    // с размером
    MutableArraySequence<int> WithSize(3);
    assert(WithSize.GetLength() == 3);

    // с данными
    int items[] = {1, 2, 3};
    MutableArraySequence<int> WithData(items, 3);
    assert(WithData.GetLength() == 3);
    assert(WithData.Get(0) == 1);
    assert(WithData.Get(2) == 3);

    printf("  [OK] testArraySequenceCreate\n");
}

void testArraySequenceGetFirstLast() {
    int items[] = {10, 20, 30};
    MutableArraySequence<int> Seq(items, 3);

    assert(Seq.GetFirst() == 10);
    assert(Seq.GetLast() == 30);

    // пустая — исключение
    MutableArraySequence<int> Empty;
    bool threw = false;
    try { Empty.GetFirst(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { Empty.GetLast(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testArraySequenceGetFirstLast\n");
}

void testArraySequenceGet() {
    int items[] = {5, 10, 15};
    MutableArraySequence<int> Seq(items, 3);

    assert(Seq.Get(0) == 5);
    assert(Seq[2] == 15);  // через оператор []

    // выход за границы
    bool threw = false;
    try { Seq.Get(5); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testArraySequenceGet\n");
}

void testArraySequenceGetSubsequence() {
    int items[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> Seq(items, 5);

    Sequence<int> *Sub = Seq.GetSubsequence(1, 3);
    assert(Sub->GetLength() == 3);
    assert(Sub->Get(0) == 2);
    assert(Sub->Get(2) == 4);
    delete Sub;

    // граничный случай: один элемент
    Sub = Seq.GetSubsequence(2, 2);
    assert(Sub->GetLength() == 1);
    assert(Sub->Get(0) == 3);
    delete Sub;

    // некорректные индексы
    bool threw = false;
    try { Seq.GetSubsequence(3, 1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { Seq.GetSubsequence(-1, 2); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testArraySequenceGetSubsequence\n");
}


void testArraySequenceConcat() {
    int a[] = {1, 2};
    int b[] = {3, 4};
    MutableArraySequence<int> First(a, 2);
    MutableArraySequence<int> Second(b, 2);

    // [1,2] + [3,4] = [1,2,3,4]
    Sequence<int>* Concatenated = First.Concat(&Second);
    assert(Concatenated->GetLength() == 4);
    assert(Concatenated->Get(0) == 1);
    assert(Concatenated->Get(3) == 4);
    delete Concatenated;

    // склеивание с пустым
    MutableArraySequence<int> Empty;
    Concatenated = First.Concat(&Empty);
    assert(Concatenated->GetLength() == 2);
    delete Concatenated;

    printf("  [OK] testArraySequenceConcat\n");
}

void testArraySequenceMap() {
    int items[] = {1, 2, 3};
    MutableArraySequence<int> Source(items, 3);

    // doubleIt: [1,2,3] -> [2,4,6]
    Sequence<int>* Doubled = Source.Map(DoubleIt);
    assert(Doubled->GetLength() == 3);
    assert(Doubled->Get(0) == 2);
    assert(Doubled->Get(1) == 4);
    assert(Doubled->Get(2) == 6);
    delete Doubled;

    // оригинал не изменился
    assert(Source.Get(0) == 1);

    printf("  [OK] testArraySequenceMap\n");
}

void testArraySequenceWhere() {
    int items[] = {1, 2, 3, 4, 5};
    MutableArraySequence<int> Mixed(items, 5);

    // isEven: [1,2,3,4,5] -> [2,4]
    Sequence<int> *EvenOnly = Mixed.Where(IsEven);
    assert(EvenOnly->GetLength() == 2);
    assert(EvenOnly->Get(0) == 2);
    assert(EvenOnly->Get(1) == 4);
    delete EvenOnly;

    // ни одного подходящего
    int odds[] = {1, 3, 5};
    MutableArraySequence<int> OnlyOdds(odds, 3);
    Sequence<int>* NoEvens = OnlyOdds.Where(IsEven);
    assert(NoEvens->GetLength() == 0);
    delete NoEvens;

    printf("  [OK] testArraySequenceWhere\n");
}

void testArraySequenceReduce() {
    int items[] = {1, 2, 3, 4};
    MutableArraySequence<int> Seq(items, 4);

    // сумма с начальным 0: 0+1+2+3+4 = 10
    int Sum = Seq.Reduce(AddInts, 0);
    assert(Sum == 10);

    // начальное значение не ноль
    Sum = Seq.Reduce(AddInts, 10);
    assert(Sum == 20);

    printf("  [OK] testArraySequenceReduce\n");
}

void testArraySequenceAll() {
    printf("=== Тесты ArraySequence (Sequence<T>) ===\n");
    testArraySequenceCreate();
    testArraySequenceGet();
    testArraySequenceGetFirstLast();
    testArraySequenceGetSubsequence();
    testArraySequenceConcat();
    testArraySequenceMap();
    testArraySequenceWhere();
    testArraySequenceReduce();
    printf("=== Все тесты пройдены! ===\n\n");
}
