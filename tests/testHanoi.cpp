#include <cassert> // assert
#include <cstdio> //printf

#include "../atd/Hanoi.h"

// собрать башню из n дисков: размеры n, n-1, ..., 1 (большой первым -> ляжет вниз)
static MutableArraySequence<Disk> makeTower(int n) {
    MutableArraySequence<Disk> disks;
    for (int sz = n; sz >= 1; sz--) {
        Disk d = { sz, Shape::Circle, Color::Red };
        disks.Append(d);
    }
    return disks;
}

// общий прогон: решить и проверить число ходов + финальное состояние
static void checkSolved(int n, int startRod, int target) {
    MutableArraySequence<Disk> disks = makeTower(n);
    Hanoi h(&disks, startRod);
    h.Solve(target);   // если бы ход был нелегальным, MoveDisk бросил бы исключение

    // ровно 2^n - 1 ходов
    assert(h.MovesCount() == (1 << n) - 1);

    // все диски на целевом стержне, в правильном порядке (низ->верх: большой->маленький)
    assert(h.Stick(target).Size() == n);
    for (int i = 0; i < n; i++)
        assert(h.Stick(target).Get(i).size == n - i);

    // остальные стержни пусты
    int aux = 3 - startRod - target;
    assert(h.Stick(startRod).IsEmpty());
    assert(h.Stick(aux).IsEmpty());
}

void testHanoiSolves() {
    checkSolved(1, 0, 2);   // граничный: один диск -> один ход
    checkSolved(3, 0, 2);
    checkSolved(4, 1, 0);   // другой старт/цель
    checkSolved(5, 2, 1);
    printf("  [OK] testHanoiSolves\n");
}

void testHanoiMoveCount() {
    // 2^n - 1 для разных n
    for (int n = 1; n <= 6; n++) {
        MutableArraySequence<Disk> disks = makeTower(n);
        Hanoi h(&disks, 0);
        h.Solve(2);
        assert(h.MovesCount() == (1 << n) - 1);
    }
    printf("  [OK] testHanoiMoveCount\n");
}

void testHanoiInvalid() {
    // items == nullptr
    bool threw = false;
    try { Hanoi h(nullptr, 0); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    // пустой список дисков
    MutableArraySequence<Disk> empty;
    threw = false;
    try { Hanoi h(&empty, 0); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    // неверный стартовый стержень
    MutableArraySequence<Disk> three = makeTower(3);
    threw = false;
    try { Hanoi h(&three, 5); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // диски не по убыванию ([1, 2])
    MutableArraySequence<Disk> bad;
    Disk a = {1, Shape::Circle, Color::Red};
    Disk b = {2, Shape::Circle, Color::Red};
    bad.Append(a); bad.Append(b);
    threw = false;
    try { Hanoi h(&bad, 0); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    // Solve: неверный target
    MutableArraySequence<Disk> t = makeTower(3);
    Hanoi h(&t, 0);
    threw = false;
    try { h.Solve(5); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // Solve: target совпадает со start
    threw = false;
    try { h.Solve(0); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testHanoiInvalid\n");
}

void testHanoiAll() {
    printf("=== Тесты Hanoi ===\n");
    testHanoiSolves();
    testHanoiMoveCount();
    testHanoiInvalid();
    printf("=== Все тесты пройдены! ===\n\n");
}
