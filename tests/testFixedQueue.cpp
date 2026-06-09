#include <cassert> // assert
#include <cstdio> //printf

#include "../atd/FixedQueue.h"

void testFixedQueueCreate() {
    // нормальный порог
    FixedQueue<int> Window(2);
    assert(Window.Size() == 0);
    assert(!Window.IsFull());

    // порог <= 0 — исключение
    bool threw = false;
    try { FixedQueue<int> Bad(0); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    threw = false;
    try { FixedQueue<int> Bad(-3); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testFixedQueueCreate\n");
}

void testFixedQueuePush() {
    // окно размера 2 хранит только 2 последних
    FixedQueue<int> Window(2);

    Window.Push(1); //[1]
    assert(Window.Size() == 1);
    assert(Window.Get(0) == 1);

    Window.Push(2); //[1, 2]
    assert(Window.Size() == 2);
    assert(Window.Get(0) == 1);
    assert(Window.Get(1) == 2);

    Window.Push(3); //[1, 2] -> [2, 3], голова 1 выкинута
    assert(Window.Size() == 2);
    assert(Window.Get(0) == 2);
    assert(Window.Get(1) == 3);

    Window.Push(5); //[2, 3] -> [3, 5], голова 2 выкинута
    assert(Window.Get(0) == 3);
    assert(Window.Get(1) == 5);

    printf("  [OK] testFixedQueuePush\n");
}

void testFixedQueueGet() {
    int items[] = {10, 20};
    FixedQueue<int> Window(3);
    for (int i = 0; i < 2; i++)
        Window.Push(items[i]);

    assert(Window.Get(0) == 10);
    assert(Window.Get(1) == 20);

    // выход за границы
    bool threw = false;
    try { Window.Get(2); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { Window.Get(-1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testFixedQueueGet\n");
}

void testFixedQueueIsFull() {
    FixedQueue<int> Window(2);
    assert(!Window.IsFull());

    Window.Push(1);
    assert(!Window.IsFull()); // 1 из 2

    Window.Push(2);
    assert(Window.IsFull()); // 2 из 2

    Window.Push(3); // окно сдвинулось, но всё ещё полное
    assert(Window.IsFull());

    printf("  [OK] testFixedQueueIsFull\n");
}

void testFixedQueueFibonacci() {
    // так генератор Фибоначчи использует окно размера 2
    FixedQueue<int> Window(2);
    Window.Push(0); // f(0)
    Window.Push(1); // f(1)

    // порождаем 8 элементов по правилу f(n) = f(n-1) + f(n-2)
    int expected[] = {1, 2, 3, 5, 8, 13, 21, 34};
    for (int i = 0; i < 8; i++) {
        int next = Window.Get(0) + Window.Get(1);
        assert(next == expected[i]);
        Window.Push(next); // окно само забывает самый старый
    }

    printf("  [OK] testFixedQueueFibonacci\n");
}

void testFixedQueueAll() {
    printf("=== Тесты FixedQueue ===\n");
    testFixedQueueCreate();
    testFixedQueuePush();
    testFixedQueueGet();
    testFixedQueueIsFull();
    testFixedQueueFibonacci();
    printf("=== Все тесты пройдены! ===\n\n");
}
