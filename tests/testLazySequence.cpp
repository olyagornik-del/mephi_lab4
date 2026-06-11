#include <cassert> // assert
#include <cstdio> //printf
#include <functional> // std::function

#include "../core/Lazy/LazySequence.h"
#include "../core/Sequences/MutableArraySequence.h"

void testLazySequenceFromArray() {
    // конечный список из массива
    int items[] = {10, 20, 30};
    LazySequence<int> List(items, 3);

    assert(!List.IsInfinite());
    assert(List.GetLength() == Ordinal::Finite(3));
    assert(List.Get(0) == 10);
    assert(List.Get(2) == 30);
    assert(List.GetFirst() == 10);
    assert(List.GetLast() == 30);

    // конечный список материализован весь сразу
    assert(List.GetMaterializedCount() == 3);

    printf("  [OK] testLazySequenceFromArray\n");
}

void testLazySequenceFromSequence() {
    // конечный список из обычной последовательности
    int items[] = {1, 2, 3, 4};
    MutableArraySequence<int> Seq(items, 4);
    LazySequence<int> List(&Seq);

    assert(List.GetLength() == Ordinal::Finite(4));
    assert(List.Get(1) == 2);
    assert(List.GetLast() == 4);

    printf("  [OK] testLazySequenceFromSequence\n");
}

void testLazySequenceEmpty() {
    LazySequence<int> Empty;
    assert(Empty.GetLength() == Ordinal::Finite(0));
    assert(Empty.GetMaterializedCount() == 0);

    // у пустого списка нет первого элемента
    bool threw = false;
    try { Empty.GetFirst(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testLazySequenceEmpty\n");
}

void testLazySequenceOutOfRange() {
    int items[] = {5, 6, 7};
    LazySequence<int> List(items, 3);

    // отрицательный индекс
    bool threw = false;
    try { List.Get(-1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // за пределами конечного списка
    threw = false;
    try { List.Get(3); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testLazySequenceOutOfRange\n");
}

void testLazySequenceFibonacci() {
    // бесконечный список Фибоначчи: f(n) = f(n-1) + f(n-2), начало {0, 1}
    int seedItems[] = {0, 1};
    MutableArraySequence<int> Seed(seedItems, 2);
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        int n = s->GetLength();
        return s->Get(n - 1) + s->Get(n - 2);
    };
    LazySequence<int> Fib(rule, &Seed);

    assert(Fib.IsInfinite());
    assert(Fib.GetLength() == Ordinal::Omega());

    // 0 1 1 2 3 5 8 13 21 34 55
    int expected[] = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34, 55};
    for (int i = 0; i <= 10; i++)
        assert(Fib.Get(i) == expected[i]);

    // последний элемент бесконечного списка недостижим
    bool threw = false;
    try { Fib.GetLast(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testLazySequenceFibonacci\n");
}

void testLazySequenceNaturals() {
    // натуральные числа из пустого начала: правило = длина уже материализованного
    MutableArraySequence<int> Seed; // пусто
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed);

    // 0 1 2 3 ... 99
    for (int i = 0; i < 100; i++)
        assert(Nat.Get(i) == i);

    printf("  [OK] testLazySequenceNaturals\n");
}

void testLazySequenceFactorial() {
    // факториалы: 0!=1, далее n! = (n-1)! * n; начало {1}
    int seedItems[] = {1};
    MutableArraySequence<int> Seed(seedItems, 1);
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        int n = s->GetLength();
        return s->Get(n - 1) * n;
    };
    LazySequence<int> Fact(rule, &Seed);

    // 1 1 2 6 24 120 720
    int expected[] = {1, 1, 2, 6, 24, 120, 720};
    for (int i = 0; i <= 6; i++)
        assert(Fact.Get(i) == expected[i]);

    printf("  [OK] testLazySequenceFactorial\n");
}

void testLazySequenceMemoization() {
    // материализация происходит лениво — только до запрошенного индекса
    int seedItems[] = {0, 1};
    MutableArraySequence<int> Seed(seedItems, 2);
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        int n = s->GetLength();
        return s->Get(n - 1) + s->Get(n - 2);
    };
    LazySequence<int> Fib(rule, &Seed);

    // вначале материализованы только начальные 2
    assert(Fib.GetMaterializedCount() == 2);

    // запрос Get(10) домотает ровно до индекса 10
    Fib.Get(10);
    assert(Fib.GetMaterializedCount() == 11);

    // повторный запрос меньшего индекса ничего не материализует
    Fib.Get(5);
    assert(Fib.GetMaterializedCount() == 11);

    printf("  [OK] testLazySequenceMemoization\n");
}

void testLazySequenceCopy() {
    // копия бесконечного списка независима и продолжает работать
    int seedItems[] = {0, 1};
    MutableArraySequence<int> Seed(seedItems, 2);
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        int n = s->GetLength();
        return s->Get(n - 1) + s->Get(n - 2);
    };
    LazySequence<int> Fib(rule, &Seed);
    Fib.Get(5); // материализовали 6 элементов

    LazySequence<int> Copy(Fib);

    // копия продолжает порождать элементы сама
    assert(Copy.Get(10) == 55);

    // оригинал не затронут материализацией копии
    assert(Fib.GetMaterializedCount() == 6);
    assert(Copy.GetMaterializedCount() == 11);

    printf("  [OK] testLazySequenceCopy\n");
}

void testLazySequenceAppendPrepend() {
    int items[] = {2, 3, 4};
    LazySequence<int> List(items, 3);

    // append в конец конечного списка
    LazySequence<int>* Appended = List.Append(5);
    assert(Appended->GetLength() == Ordinal::Finite(4));
    assert(Appended->Get(0) == 2);
    assert(Appended->Get(3) == 5);

    // prepend в начало
    LazySequence<int>* Prepended = List.Prepend(1);
    assert(Prepended->GetLength() == Ordinal::Finite(4));
    assert(Prepended->Get(0) == 1);
    assert(Prepended->Get(1) == 2);

    delete Appended;
    delete Prepended;
    printf("  [OK] testLazySequenceAppendPrepend\n");
}

void testLazySequenceConcatFinite() {
    int a[] = {1, 2, 3};
    int b[] = {4, 5};
    LazySequence<int> A(a, 3);
    LazySequence<int> B(b, 2);

    LazySequence<int>* C = A.Concat(&B);
    assert(C->GetLength() == Ordinal::Finite(5));

    // 1 2 3 4 5
    int expected[] = {1, 2, 3, 4, 5};
    for (int i = 0; i < 5; i++)
        assert(C->Get(i) == expected[i]);

    delete C;
    printf("  [OK] testLazySequenceConcatFinite\n");
}

void testLazySequenceConcatInfinite() {
    // два бесконечных ряда: A = 0,1,2,...  B = 100,101,102,...
    MutableArraySequence<int> SeedA; // пусто
    std::function<int(Sequence<int>*)> ruleA = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> A(ruleA, &SeedA);

    int seedB[] = {100};
    MutableArraySequence<int> SeedB(seedB, 1);
    std::function<int(Sequence<int>*)> ruleB = [](Sequence<int>* s) {
        return s->GetLast() + 1;
    };
    LazySequence<int> B(ruleB, &SeedB);

    LazySequence<int>* C = A.Concat(&B);

    // длина = ω + ω = ω*2
    assert(C->GetLength() == Ordinal::Omega(2));
    assert(C->IsInfinite());

    // обычные индексы — из первого сегмента (A)
    assert(C->Get(0) == 0);
    assert(C->Get(5) == 5);

    // ПРЫЖОК ЧЕРЕЗ ω: индекс ω — первый элемент B = 100
    assert(C->GetByOrdinal(Ordinal::Omega()) == 100);
    // ω+3 — элемент 3 второго сегмента B = 103
    assert(C->GetByOrdinal(Ordinal::FromParts(1, 3)) == 103);

    delete C;
    printf("  [OK] testLazySequenceConcatInfinite\n");
}

void testLazySequenceAppendToInfinite() {
    // append к бесконечному: элемент оказывается «в бесконечности» (индекс ω)
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed); // 0,1,2,...

    LazySequence<int>* Appended = Nat.Append(999);

    // длина ω + 1
    assert(Appended->GetLength() == Ordinal::FromParts(1, 1));
    // конечные индексы — из натуральных
    assert(Appended->Get(10) == 10);
    // добавленный элемент достижим только на индексе ω
    assert(Appended->GetByOrdinal(Ordinal::Omega()) == 999);

    delete Appended;
    printf("  [OK] testLazySequenceAppendToInfinite\n");
}

void testLazySequenceInsertAt() {
    // вставка в конечный список
    int items[] = {1, 2, 4};
    LazySequence<int> List(items, 3);

    LazySequence<int>* Inserted = List.InsertAt(3, 2); // вставить 3 на позицию 2
    assert(Inserted->GetLength() == Ordinal::Finite(4));

    // 1 2 4 -> 1 2 3 4
    int expected[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++)
        assert(Inserted->Get(i) == expected[i]);
    delete Inserted;

    // вставка в бесконечный список (натуральные) на конечную позицию
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed); // 0,1,2,...
    LazySequence<int>* Ins = Nat.InsertAt(99, 2); // 0,1,99,2,3,...
    assert(Ins->IsInfinite());
    assert(Ins->Get(0) == 0);
    assert(Ins->Get(1) == 1);
    assert(Ins->Get(2) == 99);
    assert(Ins->Get(3) == 2);
    assert(Ins->Get(4) == 3);
    delete Ins;

    printf("  [OK] testLazySequenceInsertAt\n");
}

void testLazySequenceMap() {
    // map конечного списка
    int items[] = {1, 2, 3};
    LazySequence<int> List(items, 3);
    std::function<int(const int&)> sq = [](const int& x) { return x * x; };
    LazySequence<int>* Squared = List.Map<int>(sq);
    assert(Squared->GetLength() == Ordinal::Finite(3));
    assert(Squared->Get(0) == 1);
    assert(Squared->Get(2) == 9);
    delete Squared;

    // map бесконечного остаётся ленивым и бесконечным
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed); // 0,1,2,...
    LazySequence<int>* NatSq = Nat.Map<int>(sq);
    assert(NatSq->IsInfinite());
    assert(NatSq->Get(5) == 25);
    assert(NatSq->Get(10) == 100);
    delete NatSq;

    printf("  [OK] testLazySequenceMap\n");
}

void testLazySequenceWhere() {
    std::function<bool(const int&)> even = [](const int& x) { return x % 2 == 0; };

    // фильтр конечного: чётные из 1..6
    int items[] = {1, 2, 3, 4, 5, 6};
    LazySequence<int> List(items, 6);
    LazySequence<int>* Evens = List.Where(even);
    assert(Evens->GetLength() == Ordinal::Finite(3));
    // 2 4 6
    assert(Evens->Get(0) == 2);
    assert(Evens->Get(1) == 4);
    assert(Evens->Get(2) == 6);
    delete Evens;

    // фильтр бесконечного: чётные натуральные
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed); // 0,1,2,...
    LazySequence<int>* NatEven = Nat.Where(even);
    assert(NatEven->IsInfinite());
    // 0 2 4 6
    assert(NatEven->Get(0) == 0);
    assert(NatEven->Get(3) == 6);
    delete NatEven;

    printf("  [OK] testLazySequenceWhere\n");
}

void testLazySequenceReduce() {
    std::function<int(const int&, const int&)> add = [](const int& a, const int& b) {
        return a + b;
    };

    // сумма конечного списка
    int items[] = {1, 2, 3, 4};
    LazySequence<int> List(items, 4);
    assert(List.Reduce<int>(add, 0) == 10);

    // на бесконечном Reduce не определён — исключение
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed);
    bool threw = false;
    try { Nat.Reduce<int>(add, 0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testLazySequenceReduce\n");
}

void testLazySequenceGetSubsequence() {
    // подпоследовательность конечного
    int items[] = {10, 20, 30, 40, 50};
    LazySequence<int> List(items, 5);
    LazySequence<int>* Sub = List.GetSubsequence(1, 3);
    assert(Sub->GetLength() == Ordinal::Finite(3));
    // 20 30 40
    assert(Sub->Get(0) == 20);
    assert(Sub->Get(2) == 40);
    delete Sub;

    // конечный диапазон из бесконечного списка
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed); // 0,1,2,...
    LazySequence<int>* Sub2 = Nat.GetSubsequence(5, 8);
    assert(Sub2->GetLength() == Ordinal::Finite(4));
    // 5 6 7 8
    assert(Sub2->Get(0) == 5);
    assert(Sub2->Get(3) == 8);
    delete Sub2;

    printf("  [OK] testLazySequenceGetSubsequence\n");
}

void testLazySequenceChained() {
    // натуральные -> квадраты -> только чётные квадраты (цепочка операций, всё лениво)
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> ruleNat = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(ruleNat, &Seed); // 0,1,2,...

    std::function<int(const int&)> sq = [](const int& x) { return x * x; };
    std::function<bool(const int&)> even = [](const int& x) { return x % 2 == 0; };

    LazySequence<int>* Squares = Nat.Map<int>(sq);          // 0,1,4,9,16,25,36,...
    LazySequence<int>* EvenSquares = Squares->Where(even);  // 0,4,16,36,...

    assert(EvenSquares->IsInfinite());
    assert(EvenSquares->Get(0) == 0);
    assert(EvenSquares->Get(1) == 4);
    assert(EvenSquares->Get(2) == 16);
    assert(EvenSquares->Get(3) == 36);

    delete Squares;
    delete EvenSquares;
    printf("  [OK] testLazySequenceChained\n");
}

void testLazySequenceNestedConcat() {
    // три бесконечных ряда: A=0..  B=100..  C=200..  → длина ω*3
    MutableArraySequence<int> SeedA;
    std::function<int(Sequence<int>*)> ruleNat = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> A(ruleNat, &SeedA);

    int sb[1] = {100};
    int sc[1] = {200};
    MutableArraySequence<int> SeedB(sb, 1);
    MutableArraySequence<int> SeedC(sc, 1);
    std::function<int(Sequence<int>*)> ruleInc = [](Sequence<int>* s) {
        return s->GetLast() + 1;
    };
    LazySequence<int> B(ruleInc, &SeedB);
    LazySequence<int> C(ruleInc, &SeedC);

    LazySequence<int>* AB = A.Concat(&B);
    LazySequence<int>* ABC = AB->Concat(&C);

    assert(ABC->GetLength() == Ordinal::Omega(3));
    assert(ABC->Get(5) == 5);                                  // A
    assert(ABC->GetByOrdinal(Ordinal::Omega()) == 100);        // B на ω
    assert(ABC->GetByOrdinal(Ordinal::Omega(2)) == 200);       // C на ω*2
    assert(ABC->GetByOrdinal(Ordinal::FromParts(2, 3)) == 203); // C, ω*2+3

    delete AB;
    delete ABC;
    printf("  [OK] testLazySequenceNestedConcat\n");
}

void testLazySequenceAll() {
    printf("=== Тесты LazySequence ===\n");
    testLazySequenceFromArray();
    testLazySequenceFromSequence();
    testLazySequenceEmpty();
    testLazySequenceOutOfRange();
    testLazySequenceFibonacci();
    testLazySequenceNaturals();
    testLazySequenceFactorial();
    testLazySequenceMemoization();
    testLazySequenceCopy();
    testLazySequenceAppendPrepend();
    testLazySequenceConcatFinite();
    testLazySequenceConcatInfinite();
    testLazySequenceAppendToInfinite();
    testLazySequenceInsertAt();
    testLazySequenceMap();
    testLazySequenceWhere();
    testLazySequenceReduce();
    testLazySequenceGetSubsequence();
    testLazySequenceChained();
    testLazySequenceNestedConcat();
    printf("=== Все тесты пройдены! ===\n\n");
}
