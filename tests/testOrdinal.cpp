#include <cassert> // assert
#include <cstdio> //printf

#include "../core/Lazy/Ordinal.h"

void testOrdinalCreate() {
    // фабрики собирают нужные части
    Ordinal Five = Ordinal::Finite(5);
    assert(Five.GetOmegaCoeff() == 0);
    assert(Five.GetFinite() == 5);

    Ordinal W = Ordinal::Omega();
    assert(W.GetOmegaCoeff() == 1);
    assert(W.GetFinite() == 0);

    Ordinal W2 = Ordinal::Omega(2);
    assert(W2.GetOmegaCoeff() == 2);
    assert(W2.GetFinite() == 0);

    Ordinal Mixed = Ordinal::FromParts(2, 3); // ω*2+3
    assert(Mixed.GetOmegaCoeff() == 2);
    assert(Mixed.GetFinite() == 3);

    // пустой конструктор — это ноль
    Ordinal Zero;
    assert(Zero.GetOmegaCoeff() == 0);
    assert(Zero.GetFinite() == 0);

    printf("  [OK] testOrdinalCreate\n");
}

void testOrdinalPredicates() {
    assert(Ordinal::Finite(0).IsZero());
    assert(!Ordinal::Finite(3).IsZero());

    // конечные
    assert(Ordinal::Finite(7).IsFinite());
    assert(!Ordinal::Finite(7).IsInfinite());

    // бесконечные
    assert(Ordinal::Omega().IsInfinite());
    assert(!Ordinal::Omega().IsFinite());
    assert(Ordinal::FromParts(1, 5).IsInfinite());

    printf("  [OK] testOrdinalPredicates\n");
}

void testOrdinalAddFinite() {
    // конечное + конечное = обычное сложение
    Ordinal Sum = Ordinal::Finite(2) + Ordinal::Finite(3);
    assert(Sum == Ordinal::Finite(5));

    // ω + конечное = прибавляем к хвосту
    Ordinal WPlus = Ordinal::Omega() + Ordinal::Finite(2); // ω+2
    assert(WPlus.GetOmegaCoeff() == 1);
    assert(WPlus.GetFinite() == 2);

    printf("  [OK] testOrdinalAddFinite\n");
}

void testOrdinalAddNonCommutative() {
    Ordinal Two = Ordinal::Finite(2);
    Ordinal W = Ordinal::Omega();

    // 2 + ω = ω — конечный хвост поглощается
    assert((Two + W) == W);

    // ω + 2 = ω+2 — хвост сохраняется
    assert((W + Two) == Ordinal::FromParts(1, 2));

    // и значит сложение НЕкоммутативно
    assert((Two + W) != (W + Two));

    // ω + ω = ω*2
    assert((W + W) == Ordinal::Omega(2));

    printf("  [OK] testOrdinalAddNonCommutative\n");
}

void testOrdinalCompare() {
    // конечное < бесконечного всегда
    assert(Ordinal::Finite(1000000) < Ordinal::Omega());

    // по коэффициенту ω
    assert(Ordinal::Omega() < Ordinal::Omega(2));

    // при равном коэффициенте — по хвосту
    assert(Ordinal::FromParts(1, 3) < Ordinal::FromParts(1, 5));

    // равенство
    assert(Ordinal::FromParts(2, 3) == Ordinal::FromParts(2, 3));
    assert(Ordinal::Finite(4) != Ordinal::Finite(5));

    // обратные операторы
    assert(Ordinal::Omega() > Ordinal::Finite(9));
    assert(Ordinal::Finite(5) <= Ordinal::Finite(5));
    assert(Ordinal::Omega(2) >= Ordinal::Omega());

    printf("  [OK] testOrdinalCompare\n");
}

void testOrdinalOverflow() {
    unsigned long long max = (unsigned long long)-1; // максимум unsigned long long

    // переполнение конечной части
    bool threw = false;
    try { Ordinal::Finite(max) + Ordinal::Finite(1); }
    catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // переполнение коэффициента ω
    threw = false;
    try { Ordinal::Omega(max) + Ordinal::Omega(1); }
    catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testOrdinalOverflow\n");
}

void testOrdinalToString() {
    assert(Ordinal::Finite(5).ToString() == "5");
    assert(Ordinal::Omega().ToString() == "ω");
    assert(Ordinal::Omega(2).ToString() == "ω*2");
    assert(Ordinal::FromParts(1, 3).ToString() == "ω+3");
    assert(Ordinal::FromParts(2, 3).ToString() == "ω*2+3");

    printf("  [OK] testOrdinalToString\n");
}

void testOrdinalAll() {
    printf("=== Тесты Ordinal ===\n");
    testOrdinalCreate();
    testOrdinalPredicates();
    testOrdinalAddFinite();
    testOrdinalAddNonCommutative();
    testOrdinalCompare();
    testOrdinalOverflow();
    testOrdinalToString();
    printf("=== Все тесты пройдены! ===\n\n");
}
