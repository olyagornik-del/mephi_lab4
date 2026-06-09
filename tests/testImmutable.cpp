#include <cassert> // assert
#include <cstdio> //printf

#include "../core/Sequences/ImmutableArraySequence.h"
#include "../core/Sequences/ImmutableListSequence.h"

static int DoubleIt(const int& x) {
    return x * 2;
}
static bool IsEven(const int& x) {
    return x % 2 == 0;
}

void testImmutableArraySequenceAppend() {
    int items[] = {1, 2, 3};
    ImmutableArraySequence<int> Seq(items, 3);

    Sequence<int> *result = Seq.Append(99);
    assert(result != &Seq);           // вернул новый объект
    assert(Seq.GetLength() == 3);     // оригинал не изменился
    assert(result->GetLength() == 4); // новый содержит добавленный элемент
    assert(result->GetLast() == 99);
    delete result;

    printf("  [OK] testImmutableArraySequenceAppend\n");
}

void testImmutableArraySequencePrepend() {
    int items[] = {1, 2, 3};
    ImmutableArraySequence<int> Seq(items, 3);

    Sequence<int> *result = Seq.Prepend(0);
    assert(result != &Seq);
    assert(Seq.GetLength() == 3);
    assert(Seq.GetFirst() == 1);      // оригинал не изменился
    assert(result->GetLength() == 4);
    assert(result->GetFirst() == 0);
    delete result;

    printf("  [OK] testImmutableArraySequencePrepend\n");
}

void testImmutableArraySequenceInsertAt() {
    int items[] = {1, 3};
    ImmutableArraySequence<int> Seq(items, 2);

    Sequence<int> *result = Seq.InsertAt(1, 2);
    assert(result != &Seq);
    assert(Seq.GetLength() == 2);     // оригинал не изменился
    assert(Seq.Get(1) == 3);
    assert(result->GetLength() == 3);
    assert(result->Get(1) == 2);
    delete result;

    bool threw = false;
    try { Seq.InsertAt(-1, 0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testImmutableArraySequenceInsertAt\n");
}

void testImmutableArraySequenceMap() {
    int items[] = {1, 2, 3};
    ImmutableArraySequence<int> Seq(items, 3);

    Sequence<int> *mapped = Seq.Map(DoubleIt);
    assert(mapped->GetLength() == 3);
    assert(mapped->Get(0) == 2);
    assert(mapped->Get(2) == 6);
    assert(Seq.Get(0) == 1); // проверка, что оригинал цел

    // проверка, что результат тоже Imm
    Sequence<int> *after = mapped->Append(99);
    assert(after != mapped); //Append должен вернуть копию
    assert(mapped->GetLength() == 3); // mapped не изменился
    delete after;
    delete mapped;

    printf("  [OK] testImmutableArraySequenceMap\n");
}

void testImmutableArraySequenceWhere() {
    int items[] = {1, 2, 3};
    ImmutableArraySequence<int> Seq(items, 3);

    //[1, 2, 3] -> [2]
    Sequence<int> *was_where = Seq.Where(IsEven);
    assert(was_where->GetLength() == 1);
    assert(was_where->GetFirst() == 2);
    assert(Seq.GetLength() == 3);

    //проверка, что результат тоже Imm
    Sequence<int> *after = was_where->Where(IsEven);
    assert(after != was_where);
    assert(was_where->GetLength() == 1);
    delete after;
    delete was_where;

    printf("  [OK] testImmutableArraySequenceWhere\n");
}

void testImmutableArraySequenceConcat() {
    int a[] = {1, 2};
    int b[] = {3, 4};
    ImmutableArraySequence<int> First(a, 2);
    ImmutableArraySequence<int> Second(b, 2);

    // [1,2] + [3,4] = [1,2,3,4]
    Sequence<int> *Concatenated = First.Concat(&Second);
    assert(Concatenated->GetLength() == 4);
    assert(Concatenated->Get(0) == 1);
    assert(Concatenated->Get(3) == 4);

    // оригиналы не изменились
    assert(First.GetLength() == 2);
    assert(Second.GetLength() == 2);
    assert(First.Get(0) == 1);

    // проверка, что результат тоже Immutable
    Sequence<int> *after = Concatenated->Append(99);
    assert(after != Concatenated); // Append должен вернунь КОПИЮ
    assert(Concatenated->GetLength() == 4); // сам Concatenated не изменился
    assert(after->GetLength() == 5);
    delete after;
    delete Concatenated;

    // склеивание с пустым
    ImmutableArraySequence<int> Empty;
    Concatenated = First.Concat(&Empty);
    assert(Concatenated->GetLength() == 2);
    assert(Concatenated->Get(0) == 1);
    delete Concatenated;

    printf("  [OK] testImmutableArraySequenceConcat\n");
}

void testImmutableArraySequenceGetSubsequence() {
    int items[] = {1, 2, 3, 4, 5};
    ImmutableArraySequence<int> Seq(items, 5);

    // подпоследовательность [1..3] -> [2,3,4]
    Sequence<int> *Sub = Seq.GetSubsequence(1, 3);
    assert(Sub->GetLength() == 3);
    assert(Sub->Get(0) == 2);
    assert(Sub->Get(2) == 4);

    // оригинал не изменился
    assert(Seq.GetLength() == 5);
    assert(Seq.Get(0) == 1);

    // проверка, что результат тоже Immutable
    Sequence<int> *after = Sub->Append(99);
    assert(after != Sub);              // Append должен вернуть КОПИЮ
    assert(Sub->GetLength() == 3);     // сам Sub не изменился
    assert(after->GetLength() == 4);
    delete after;
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

    printf("  [OK] testImmutableArraySequenceGetSubsequence\n");
}

void testImmutableListSequenceAppend() {
    int items[] = {1, 2, 3};
    ImmutableListSequence<int> Seq(items, 3);

    Sequence<int> *result = Seq.Append(99);
    assert(result != &Seq);
    assert(Seq.GetLength() == 3);
    assert(result->GetLength() == 4);
    assert(result->GetLast() == 99);
    delete result;

    printf("  [OK] testImmutableListSequenceAppend\n");
}

void testImmutableListSequencePrepend() {
    int items[] = {1, 2, 3};
    ImmutableListSequence<int> Seq(items, 3);

    Sequence<int> *result = Seq.Prepend(0);
    assert(result != &Seq);
    assert(Seq.GetLength() == 3);
    assert(Seq.GetFirst() == 1);
    assert(result->GetLength() == 4);
    assert(result->GetFirst() == 0);
    delete result;

    printf("  [OK] testImmutableListSequencePrepend\n");
}

void testImmutableListSequenceInsertAt() {
    int items[] = {1, 3};
    ImmutableListSequence<int> Seq(items, 2);

    Sequence<int> *result = Seq.InsertAt(1, 2);
    assert(result != &Seq);
    assert(Seq.GetLength() == 2);
    assert(Seq.Get(1) == 3);
    assert(result->GetLength() == 3);
    assert(result->Get(1) == 2);
    delete result;

    bool threw = false;
    try { Seq.InsertAt(-1, 0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testImmutableListSequenceInsertAt\n");
}


void testImmutableListSequenceMap() {
    int items[] = {1, 2, 3};
    ImmutableListSequence<int> Seq(items, 3);

    Sequence<int> *mapped = Seq.Map(DoubleIt);
    assert(mapped->GetLength() == 3);
    assert(mapped->Get(0) == 2);
    assert(mapped->Get(2) == 6);
    assert(Seq.Get(0) == 1); // оригинал цел

    // проверка, что результат тоже Immutable
    Sequence<int> *after = mapped->Append(99);
    assert(after != mapped);
    assert(mapped->GetLength() == 3);
    delete after;
    delete mapped;

    printf("  [OK] testImmutableListSequenceMap\n");
}

void testImmutableListSequenceWhere() {
    int items[] = {1, 2, 3};
    ImmutableListSequence<int> Seq(items, 3);

    //[1, 2, 3] -> [2]
    Sequence<int> *was_where = Seq.Where(IsEven);
    assert(was_where->GetLength() == 1);
    assert(was_where->GetFirst() == 2);
    assert(Seq.GetLength() == 3);

    //проверка, что результат тоже Imm
    Sequence<int> *after = was_where->Where(IsEven);
    assert(after != was_where);
    assert(was_where->GetLength() == 1);
    delete after;
    delete was_where;

    printf("  [OK] testImmutableListSequenceWhere\n");
}

void testImmutableListSequenceConcat() {
    int a[] = {1, 2};
    int b[] = {3, 4};
    ImmutableListSequence<int> First(a, 2);
    ImmutableListSequence<int> Second(b, 2);

    // [1,2] + [3,4] = [1,2,3,4]
    Sequence<int> *Concatenated = First.Concat(&Second);
    assert(Concatenated->GetLength() == 4);
    assert(Concatenated->Get(0) == 1);
    assert(Concatenated->Get(3) == 4);

    // оригиналы не изменились
    assert(First.GetLength() == 2);
    assert(Second.GetLength() == 2);
    assert(First.Get(0) == 1);

    // проверка, что результат тоже Immutable
    Sequence<int> *after = Concatenated->Append(99);
    assert(after != Concatenated); // Append должен вернунь КОПИЮ
    assert(Concatenated->GetLength() == 4); // сам Concatenated не изменился
    assert(after->GetLength() == 5);
    delete after;
    delete Concatenated;

    // склеивание с пустым
    ImmutableListSequence<int> Empty;
    Concatenated = First.Concat(&Empty);
    assert(Concatenated->GetLength() == 2);
    assert(Concatenated->Get(0) == 1);
    delete Concatenated;

    printf("  [OK] testImmutableListSequenceConcat\n");
}

void testImmutableListSequenceGetSubsequence() {
    int items[] = {1, 2, 3, 4, 5};
    ImmutableListSequence<int> Seq(items, 5);

    // подпоследовательность [1..3] -> [2,3,4]
    Sequence<int> *Sub = Seq.GetSubsequence(1, 3);
    assert(Sub->GetLength() == 3);
    assert(Sub->Get(0) == 2);
    assert(Sub->Get(2) == 4);

    // оригинал не изменился
    assert(Seq.GetLength() == 5);
    assert(Seq.Get(0) == 1);

    // проверка, что результат тоже Immutable
    Sequence<int> *after = Sub->Append(99);
    assert(after != Sub);              // Append должен вернуть КОПИЮ
    assert(Sub->GetLength() == 3);     // сам Sub не изменился
    assert(after->GetLength() == 4);
    delete after;
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

    printf("  [OK] testImmutableListSequenceGetSubsequence\n");
}

void testImmutableAll() {
    printf("=== Тесты Immutable (Array + List) ===\n");
    testImmutableArraySequenceAppend();
    testImmutableArraySequencePrepend();
    testImmutableArraySequenceInsertAt();
    testImmutableArraySequenceMap();
    testImmutableArraySequenceConcat();
    testImmutableArraySequenceGetSubsequence();
    testImmutableArraySequenceWhere();
    printf("== Array тесты пройдены! ==\n");
    testImmutableListSequenceAppend();
    testImmutableListSequencePrepend();
    testImmutableListSequenceInsertAt();
    testImmutableListSequenceMap();
    testImmutableListSequenceWhere();
    testImmutableListSequenceConcat();
    testImmutableListSequenceGetSubsequence();
    printf("== List тесты пройдены! ==\n");
    printf("=== Все тесты пройдены! ===\n\n");
}
