#include <cassert> // assert
#include <cstdio> //printf

#include "../core/Sequences/MutableListSequence.h"

static int OneAnd(const int& x) {
    return x * 2;
}
static bool IsEven(const int& x) {
    return x % 2 == 0;
}
static int AddInts(const int& a, const int& b) {
    return a + b;
}

void testListSequenceCreate() {
    // пустая последовательность
    MutableListSequence<int> Empty;
    assert(Empty.GetLength() == 0);

    // с данными
    int items[] = {1, 2, 3};
    MutableListSequence<int> WithData(items, 3);
    assert(WithData.GetLength() == 3);
    assert(WithData.Get(0) == 1);
    assert(WithData.Get(2) == 3);

    printf("  [OK] testListSequenceCreate\n");
}

void testListSequenceGetFirstLast() {
    int items[] = {10, 20, 30};
    MutableListSequence<int> Seq(items, 3);

    assert(Seq.GetFirst() == 10);
    assert(Seq.GetLast() == 30);

    // пустая — исключение
    MutableListSequence<int> Empty;
    bool threw = false;
    try { Empty.GetFirst(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { Empty.GetLast(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testListSequenceGetFirstLast\n");
}

void testListSequenceGet() {
    int items[] = {5, 10, 15};
    MutableListSequence<int> Seq(items, 3);

    assert(Seq.Get(0) == 5);
    assert(Seq[2] == 15);

    // выход за границы
    bool threw = false;
    try { Seq.Get(5); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testListSequenceGet\n");
}

void testListSequenceGetSubsequence() {
    int items[] = {1, 2, 3, 4, 5};
    MutableListSequence<int> Seq(items, 5);

    Sequence<int>* Sub = Seq.GetSubsequence(1, 3);
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

    printf("  [OK] testListSequenceGetSubsequence\n");
}

void testListSequenceConcat() {
    int a[] = {1, 2};
    int b[] = {3, 4};
    MutableListSequence<int> First(a, 2);
    MutableListSequence<int> Second(b, 2);

    // [1,2] + [3,4] = [1,2,3,4]
    Sequence<int>* Concatenated = First.Concat(&Second);
    assert(Concatenated->GetLength() == 4);
    assert(Concatenated->Get(0) == 1);
    assert(Concatenated->Get(3) == 4);
    delete Concatenated;

    // склеивание с пустым
    MutableListSequence<int> Empty;
    Concatenated = First.Concat(&Empty);
    assert(Concatenated->GetLength() == 2);
    delete Concatenated;

    printf("  [OK] testListSequenceConcat\n");
}

void testListSequenceMap() {
    int items[] = {1, 2, 3};
    MutableListSequence<int> Source(items, 3);

    // doubleIt: [1,2,3] -> [2,4,6]
    Sequence<int>* Doubled = Source.Map(OneAnd);
    assert(Doubled->GetLength() == 3);
    assert(Doubled->Get(0) == 2);
    assert(Doubled->Get(1) == 4);
    assert(Doubled->Get(2) == 6);
    delete Doubled;

    // оригинал не изменился
    assert(Source.Get(0) == 1);

    printf("  [OK] testListSequenceMap\n");
}

void testListSequenceWhere() {
    int items[] = {1, 2, 3, 4, 5};
    MutableListSequence<int> Mixed(items, 5);

    // isEven: [1,2,3,4,5] -> [2,4]
    Sequence<int>* EvenOnly = Mixed.Where(IsEven);
    assert(EvenOnly->GetLength() == 2);
    assert(EvenOnly->Get(0) == 2);
    assert(EvenOnly->Get(1) == 4);
    delete EvenOnly;

    // ни одного подходящего
    int odds[] = {1, 3, 5};
    MutableListSequence<int> OnlyOdds(odds, 3);
    Sequence<int>* NoEvens = OnlyOdds.Where(IsEven);
    assert(NoEvens->GetLength() == 0);
    delete NoEvens;

    printf("  [OK] testListSequenceWhere\n");
}

void testListSequenceReduce() {
    int items[] = {1, 2, 3, 4};
    MutableListSequence<int> Seq(items, 4);

    // сумма с начальным 0: 0+1+2+3+4 = 10
    int Sum = Seq.Reduce(AddInts, 0);
    assert(Sum == 10);

    // начальное значение не ноль
    Sum = Seq.Reduce(AddInts, 10);
    assert(Sum == 20);

    printf("  [OK] testListSequenceReduce\n");
}

void testListSequenceAll() {
    printf("=== Тесты ListSequence (Sequence<T>) ===\n");
    testListSequenceCreate();
    testListSequenceGet();
    testListSequenceGetFirstLast();
    testListSequenceGetSubsequence();
    testListSequenceConcat();
    testListSequenceMap();
    testListSequenceWhere();
    testListSequenceReduce();
    printf("=== Все тесты пройдены! ===\n\n");
}
