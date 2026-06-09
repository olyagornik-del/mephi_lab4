#include <cassert> // assert
#include <cstdio> //printf

#include "../core/Sequences/BitSequence.h"

//testArraySequence вспомогательные функции для Map / Where / Reduce
static Bit InvertBit(const Bit& b) {
    return !b;
}
static bool IsTrue(const Bit& b) {
    return b;
}
static Bit AndBits(const Bit& a, const Bit& b) {
    return a && b;
}

void testBitSequenceCreate() {
    //пустая последовательность
    BitSequence Empy;
    assert(Empy.GetLength() == 0);

    // размером
    BitSequence WithSize(3);
    assert(WithSize.GetLength()==3);

    //с данными
    bool items[] = {true, false, false};
    BitSequence WithData(items, 3);
    assert(WithData.GetLength() == 3);
    assert(WithData.Get(0) == 1);
    assert(WithData.Get(2) == 0);

    printf("  [OK] testBitSequenceCreate\n");
}

void testBitSequenceFirstLast() {
    bool items[] = {false, false, true, false, true};
    BitSequence BitSeq(items, 5);

    assert(BitSeq.GetFirst() == 0);
    assert(BitSeq.GetLast() == 1);

    //пустой список
    BitSequence Empty;
    bool threw = false;
    try { Empty.GetFirst(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { Empty.GetLast(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);
    printf("  [OK] testBitSequenceGetFirstLast\n");
}
void testBitSequenceGet() {
    bool items[] = {false, false, true, false, true};
    BitSequence BitSeq(items, 5);

    assert(BitSeq.Get(0) == 0);
    assert(BitSeq[2] == 1);

    //вход за границу
    bool threw = false;
    try { BitSeq.Get(6); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testBitSequenceGet\n");
}

void testBitSequenceGetSubsequence() {
    bool items[] = {false, false, true, false, true};
    BitSequence BitSeq(items, 5);

    Sequence<Bit> *Sub = BitSeq.GetSubsequence(1, 3);
    assert(Sub->GetLength() == 3);
    assert(Sub->GetFirst() == 0);
    assert(Sub->GetLast() == 0);
    delete Sub;

    //граничные случаи
    Sub = BitSeq.GetSubsequence(2, 2);
    assert(Sub->GetLength() == 1);
    assert(Sub->Get(0) == 1);
    delete Sub;

    //некорректные индексы
    bool threw = false;
    try { BitSeq.GetSubsequence(3, 1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { BitSeq.GetSubsequence(-1, 2); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testBitSequenceGetSubsequence\n");
}

void testBitSequenceConcat() {
    bool a[] = {true, false};
    bool b[] = {false, true, false};
    BitSequence First(a, 2);
    BitSequence Second(b, 3);

    // [1,0] + [0,1] = [1, 0, 0, 1]
    Sequence<Bit>* Concatenated = First.Concat(&Second);
    assert(Concatenated->GetLength() == 5);
    assert(Concatenated->Get(0) == 1);
    assert(Concatenated->Get(4) == 0);
    delete Concatenated;

    // склеивание с пустым
    BitSequence Empty;
    Concatenated = First.Concat(&Empty);
    assert(Concatenated->GetLength() == 2);
    delete Concatenated;

    printf("  [OK] testBitSequenceConcat\n");
}

void testBitSequenceMap() {
    bool items[] = {true, false, false};
    BitSequence Source(items, 3);

    // doubleIt: [1,0,0] -> [0,1,1]
    Sequence<Bit> *Doubled = Source.Map(InvertBit);
    assert(Doubled->GetLength() == 3);
    assert(Doubled->Get(0) == 0);
    assert(Doubled->Get(1) == 1);
    assert(Doubled->Get(2) == 1);
    delete Doubled;

    // оригинал не изменился
    assert(Source.Get(0) == 1);

    printf("  [OK] testBitSequenceMap\n");
}

void testBitSequenceWhere() {
    bool items[] = {true, false, false, true, false};
    BitSequence Mixed(items, 5);

    // isEven: [1, 0, 0, 1, 0] -> [1,1]
    Sequence<Bit> *EvenOnly = Mixed.Where(IsTrue);
    assert(EvenOnly->GetLength() == 2);
    assert(EvenOnly->Get(0) == 1);
    assert(EvenOnly->Get(1) == 1);
    delete EvenOnly;

    // ни одного подходящего
    bool all_false[] = {false, false, false};
    BitSequence OnlyOdds(all_false, 3);
    Sequence<Bit> *NoTrue  = OnlyOdds.Where(IsTrue);
    assert(NoTrue->GetLength() == 0);
    delete NoTrue;

    printf("  [OK] testBitSequenceWhere\n");
}

void testBitSequenceReduce() {
    bool items[] = {true, true, true, true};
    BitSequence Seq(items, 4);

    // сумма с начальным 1: 1&&1&&1&&0&&0 = 0
    int Sum = Seq.Reduce(AndBits, 1);
    assert(Sum == 1);

    // начальное значение 0
    Sum = Seq.Reduce(AndBits, 0);
    assert(Sum == 0);

    printf("  [OK] testBitSequenceReduce\n");
}

void testBitSequenceAnd() {
    bool a[] = {0, 0, 1, 1};
    bool b[] = {0, 1, 0, 1};
    bool c[] = {0, 1};
    BitSequence First(a, 4);
    BitSequence Second(b, 4);
    BitSequence DiffrentLength(c, 2);

    BitSequence Empty;
    bool threw = false;
    try { First.And(&Empty); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    threw = false;
    try { First.And(&DiffrentLength); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    //0011&0101 = 0001
    BitSequence *res = First.And(&Second);
    assert(res->GetLength() == 4);
    assert(res->Get(0) == 0);
    assert(res->Get(1) == 0);
    assert(res->Get(2) == 0);
    assert(res->Get(3) == 1);
    delete res;

    printf("  [OK] testBitSequenceAnd\n");
}

void testBitSequenceOr() {
    bool a[] = {0, 0, 1, 1};
    bool b[] = {0, 1, 0, 1};
    bool c[] = {0, 1};
    BitSequence First(a, 4);
    BitSequence Second(b, 4);
    BitSequence DiffrentLength(c, 2);

    BitSequence Empty;
    bool threw = false;
    try { First.Or(&Empty); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    threw = false;
    try { First.Or(&DiffrentLength); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    //0011&0101 = 0111
    BitSequence *res = First.Or(&Second);
    assert(res->GetLength() == 4);
    assert(res->Get(0) == 0);
    assert(res->Get(1) == 1);
    assert(res->Get(2) == 1);
    assert(res->Get(3) == 1);
    delete res;

    printf("  [OK] testBitSequenceOr\n");
}

void testBitSequenceXor() {
    bool a[] = {0, 0, 1, 1};
    bool b[] = {0, 1, 0, 1};
    bool c[] = {0, 1};
    BitSequence First(a, 4);
    BitSequence Second(b, 4);
    BitSequence DiffrentLength(c, 2);

    BitSequence Empty;
    bool threw = false;
    try { First.Xor(&Empty); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    threw = false;
    try { First.Xor(&DiffrentLength); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    //0011&0101 = 0110
    BitSequence *res = First.Xor(&Second);
    assert(res->GetLength() == 4);
    assert(res->Get(0) == 0);
    assert(res->Get(1) == 1);
    assert(res->Get(2) == 1);
    assert(res->Get(3) == 0);
    delete res;

    printf("  [OK] testBitSequenceXor\n");
}

void testBitSequenceNot() {
    bool a[] = {0, 0, 1, 1};
    BitSequence BitSeq(a, 4);

    //[0, 0, 1, 1] -> [1, 1, 0, 0]
    BitSequence *Inverted = BitSeq.Not();
    assert(Inverted->GetLength() == 4);
    assert(Inverted->Get(0) == 1);
    assert(Inverted->Get(1) == 1);
    assert(Inverted->Get(2) == 0);
    assert(Inverted->Get(3) == 0);
    delete Inverted;

    //двойное отрицание - чтоб проверить чистку хвоста
    BitSequence *Once = BitSeq.Not();
    BitSequence *Twice = Once->Not();
    assert(Twice->GetLength() == 4);
    for (int i = 0; i < 4; i++)
        assert(Twice->Get(i) == BitSeq.Get(i));
    delete Once;
    delete Twice;

    //ровно 32 бита
    bool full[32];
    for (int i = 0; i < 32; i++) full[i] = (i % 2 == 0); //заполнение
    BitSequence Full(full, 32);
    BitSequence *InvertedFull = Full.Not();
    assert(InvertedFull->GetLength() == 32);
    for (int i = 0; i < 32; i++)
        assert(InvertedFull->Get(i) == (i % 2 == 1));
    delete InvertedFull;

    //ровно 33 бита (те 2 блока и во втором ток 1й бит нужен)
    bool tricky[33] = {0}; // все нули
    tricky[32] = true; // последний — единица
    BitSequence Tricky(tricky, 33);
    BitSequence *InvertedTricky = Tricky.Not();
    assert(InvertedTricky->Get(0) == true);
    assert(InvertedTricky->Get(31) == true);
    assert(InvertedTricky->Get(32) == false);
    delete InvertedTricky;

    printf("  [OK] testBitSequenceNot\n");
}

void testBitSequenceAll() {
    printf("=== Тесты BitSequence ===\n");
    testBitSequenceCreate();
    testBitSequenceGet();
    testBitSequenceFirstLast();
    testBitSequenceGetSubsequence();
    testBitSequenceConcat();
    testBitSequenceMap();
    testBitSequenceWhere();
    testBitSequenceReduce();
    testBitSequenceAnd();
    testBitSequenceOr();
    testBitSequenceXor();
    testBitSequenceNot();
    printf("=== Все тесты пройдены! ===\n\n");
}

