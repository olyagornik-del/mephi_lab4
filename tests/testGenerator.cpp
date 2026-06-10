#include <cassert> // assert
#include <cstdio> //printf
#include <functional> // std::function

#include "../core/Generator.h"
#include "../core/LazySequenceGenerators.h"
#include "../core/Sequences/MutableArraySequence.h"

// натуральные 0,1,2,... (рекуррентный генератор, seed пуст)
static Generator<int>* MakeNaturals() {
    MutableArraySequence<int> seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) { return s->GetLength(); };
    return new RecurrenceGenerator<int>(rule, &seed);
}

// натуральные начиная с start: start, start+1, ...
static Generator<int>* MakeNaturalsFrom(int start) {
    int s0[1] = { start };
    MutableArraySequence<int> seed(s0, 1);
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) { return s->GetLast() + 1; };
    return new RecurrenceGenerator<int>(rule, &seed);
}

void testSequenceGenerator() {
    int items[] = {10, 20, 30};
    Generator<int>* Gen = new SequenceGenerator<int>(items, 3);

    assert(Gen->GetLength() == Ordinal::Finite(3));
    assert(Gen->GetMaterializedCount() == 3); // конечный — весь известен сразу
    assert(Gen->Get(Ordinal::Finite(0)) == 10);
    assert(Gen->Get(Ordinal::Finite(2)) == 30);

    // бесконечный индекс недостижим
    bool threw = false;
    try { Gen->Get(Ordinal::Omega()); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // выход за границы
    threw = false;
    try { Gen->Get(Ordinal::Finite(3)); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // Clone — независимая копия с теми же значениями
    Generator<int>* Clone = Gen->Clone();
    assert(Clone->Get(Ordinal::Finite(1)) == 20);

    delete Gen;
    delete Clone;
    printf("  [OK] testSequenceGenerator\n");
}

void testRecurrenceGenerator() {
    Generator<int>* Nat = MakeNaturals();

    assert(Nat->GetLength() == Ordinal::Omega());
    assert(Nat->GetMaterializedCount() == 0); // ещё ничего не посчитано

    // материализация растёт лениво
    assert(Nat->Get(Ordinal::Finite(5)) == 5);
    assert(Nat->GetMaterializedCount() == 6);
    Nat->Get(Ordinal::Finite(2));
    assert(Nat->GetMaterializedCount() == 6); // меньший индекс ничего не добавил

    // бесконечный индекс недостижим
    bool threw = false;
    try { Nat->Get(Ordinal::Omega()); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // Clone копирует уже посчитанное и независим
    Generator<int>* Clone = Nat->Clone();
    assert(Clone->GetMaterializedCount() == 6);
    Clone->Get(Ordinal::Finite(10));
    assert(Clone->GetMaterializedCount() == 11);
    assert(Nat->GetMaterializedCount() == 6); // оригинал не затронут

    delete Nat;
    delete Clone;
    printf("  [OK] testRecurrenceGenerator\n");
}

void testConcatGenerator() {
    // два бесконечных: 0,1,2,...  и  100,101,102,...
    Generator<int>* Concat = new ConcatGenerator<int>(MakeNaturals(), MakeNaturalsFrom(100));

    // длина ω + ω = ω*2
    assert(Concat->GetLength() == Ordinal::Omega(2));
    // обычный индекс — из левого сегмента
    assert(Concat->Get(Ordinal::Finite(5)) == 5);
    // ПРЫЖОК через ω — в правый сегмент
    assert(Concat->Get(Ordinal::Omega()) == 100);
    assert(Concat->Get(Ordinal::FromParts(1, 3)) == 103);
    delete Concat;

    // конечный + бесконечный: [1,2,3] ++ натуральные
    int head[] = {1, 2, 3};
    Generator<int>* Mixed = new ConcatGenerator<int>(
        new SequenceGenerator<int>(head, 3), MakeNaturals());
    assert(Mixed->GetLength() == Ordinal::Omega()); // 3 + ω = ω
    assert(Mixed->Get(Ordinal::Finite(0)) == 1);
    assert(Mixed->Get(Ordinal::Finite(3)) == 0); // первый элемент правого (натуральные с 0)
    assert(Mixed->Get(Ordinal::Finite(5)) == 2);
    delete Mixed;

    printf("  [OK] testConcatGenerator\n");
}

void testInsertGenerator() {
    // вставка в конечный: [1,2,4] вставить 3 на позицию 2 -> 1,2,3,4
    int items[] = {1, 2, 4};
    Generator<int>* Ins = new InsertGenerator<int>(new SequenceGenerator<int>(items, 3), 2, 3);
    assert(Ins->GetLength() == Ordinal::Finite(4));
    assert(Ins->Get(Ordinal::Finite(0)) == 1);
    assert(Ins->Get(Ordinal::Finite(2)) == 3);
    assert(Ins->Get(Ordinal::Finite(3)) == 4);
    delete Ins;

    // вставка в бесконечный (натуральные) на позицию 2 -> 0,1,99,2,3,...
    Generator<int>* Ins2 = new InsertGenerator<int>(MakeNaturals(), 2, 99);
    assert(Ins2->GetLength() == Ordinal::Omega()); // длина не меняется
    assert(Ins2->Get(Ordinal::Finite(2)) == 99);
    assert(Ins2->Get(Ordinal::Finite(3)) == 2);
    delete Ins2;

    printf("  [OK] testInsertGenerator\n");
}

void testMapGenerator() {
    // квадраты натуральных
    std::function<int(const int&)> sq = [](const int& x) { return x * x; };
    Generator<int>* Sq = new MapGenerator<int, int>(MakeNaturals(), sq);
    assert(Sq->GetLength() == Ordinal::Omega());
    assert(Sq->Get(Ordinal::Finite(5)) == 25);
    assert(Sq->Get(Ordinal::Finite(10)) == 100);

    Generator<int>* Clone = Sq->Clone();
    assert(Clone->Get(Ordinal::Finite(4)) == 16);

    delete Sq;
    delete Clone;
    printf("  [OK] testMapGenerator\n");
}

void testWhereGenerator() {
    std::function<bool(const int&)> even = [](const int& x) { return x % 2 == 0; };

    // фильтр конечного: чётные из 1..6 -> 2,4,6
    int items[] = {1, 2, 3, 4, 5, 6};
    Generator<int>* Evens = new WhereGenerator<int>(new SequenceGenerator<int>(items, 6), even);
    assert(Evens->GetLength() == Ordinal::Finite(3));
    assert(Evens->Get(Ordinal::Finite(0)) == 2);
    assert(Evens->Get(Ordinal::Finite(2)) == 6);
    delete Evens;

    // фильтр бесконечного: чётные натуральные -> 0,2,4,6,...
    Generator<int>* NatEven = new WhereGenerator<int>(MakeNaturals(), even);
    assert(NatEven->GetLength() == Ordinal::Omega());
    assert(NatEven->Get(Ordinal::Finite(3)) == 6);
    delete NatEven;

    printf("  [OK] testWhereGenerator\n");
}

void testSubsequenceGenerator() {
    // [5,6,7,8] из натуральных: start=5, count=4
    Generator<int>* Sub = new SubsequenceGenerator<int>(MakeNaturals(), 5, 4);
    assert(Sub->GetLength() == Ordinal::Finite(4));
    assert(Sub->Get(Ordinal::Finite(0)) == 5);
    assert(Sub->Get(Ordinal::Finite(3)) == 8);

    // выход за границы
    bool threw = false;
    try { Sub->Get(Ordinal::Finite(4)); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    delete Sub;
    printf("  [OK] testSubsequenceGenerator\n");
}

void testGeneratorAll() {
    printf("=== Тесты Generator ===\n");
    testSequenceGenerator();
    testRecurrenceGenerator();
    testConcatGenerator();
    testInsertGenerator();
    testMapGenerator();
    testWhereGenerator();
    testSubsequenceGenerator();
    printf("=== Все тесты пройдены! ===\n\n");
}
