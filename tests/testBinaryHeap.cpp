#include <cassert> // assert
#include <cstdio> //printf
#include <functional> // std::greater

#include "../atd/BinaryHeap.h"

void testBinaryHeapInsertExtract() {
    // вставляем вперемешку, извлекаем — выходит по возрастанию (min-куча)
    BinaryHeap<int> Heap;
    int items[] = {5, 2, 8, 1, 9, 3};
    for (int i = 0; i < 6; i++)
        Heap.Insert(items[i]);
    assert(Heap.Size() == 6);

    //[5, 2, 8, 1, 9, 3] -> 1 2 3 5 8 9
    assert(Heap.ExtractMinOrMax() == 1);
    assert(Heap.ExtractMinOrMax() == 2);
    assert(Heap.ExtractMinOrMax() == 3);
    assert(Heap.ExtractMinOrMax() == 5);
    assert(Heap.ExtractMinOrMax() == 8);
    assert(Heap.ExtractMinOrMax() == 9);
    assert(Heap.IsEmpty());

    printf("  [OK] testBinaryHeapInsertExtract\n");
}

void testBinaryHeapMaxHeap() {
    // та же куча через std::greater — извлекаем по убыванию (max-куча)
    BinaryHeap<int, std::greater<int>> Heap;
    int items[] = {5, 2, 8, 1, 9, 3};
    for (int i = 0; i < 6; i++)
        Heap.Insert(items[i]);

    //[5, 2, 8, 1, 9, 3] -> 9 8 5 3 2 1
    assert(Heap.ExtractMinOrMax() == 9);
    assert(Heap.ExtractMinOrMax() == 8);
    assert(Heap.ExtractMinOrMax() == 5);
    assert(Heap.ExtractMinOrMax() == 3);
    assert(Heap.ExtractMinOrMax() == 2);
    assert(Heap.ExtractMinOrMax() == 1);
    assert(Heap.IsEmpty());

    printf("  [OK] testBinaryHeapMaxHeap\n");
}

void testBinaryHeapTop() {
    int items[] = {7, 3, 5};
    BinaryHeap<int> Heap;
    for (int i = 0; i < 3; i++)
        Heap.Insert(items[i]);

    // Top показывает минимум, но не удаляет его
    assert(Heap.Top() == 3);
    assert(Heap.Size() == 3);
    assert(Heap.Top() == 3); // всё ещё на месте

    // Top на пустой куче — исключение
    BinaryHeap<int> Empty;
    bool threw = false;
    try { Empty.Top(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testBinaryHeapTop\n");
}

void testBinaryHeapDuplicates() {
    // дубликаты должны сохраняться все
    int items[] = {4, 4, 1, 4, 1};
    BinaryHeap<int> Heap;
    for (int i = 0; i < 5; i++)
        Heap.Insert(items[i]);
    assert(Heap.Size() == 5);

    //[4, 4, 1, 4, 1] -> 1 1 4 4 4
    assert(Heap.ExtractMinOrMax() == 1);
    assert(Heap.ExtractMinOrMax() == 1);
    assert(Heap.ExtractMinOrMax() == 4);
    assert(Heap.ExtractMinOrMax() == 4);
    assert(Heap.ExtractMinOrMax() == 4);

    printf("  [OK] testBinaryHeapDuplicates\n");
}

void testBinaryHeapEmpty() {
    BinaryHeap<int> Empty;
    assert(Empty.IsEmpty());
    assert(Empty.Size() == 0);

    // извлечение из пустой кучи — исключение
    bool threw = false;
    try { Empty.ExtractMinOrMax(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // один элемент: вставка и извлечение
    Empty.Insert(42);
    assert(!Empty.IsEmpty());
    assert(Empty.ExtractMinOrMax() == 42);
    assert(Empty.IsEmpty());

    printf("  [OK] testBinaryHeapEmpty\n");
}

void testBinaryHeapStress() {
    // предельное значение: 100000 элементов
    BinaryHeap<int> Heap;
    int count = 100000;
    // простой генератор псевдослучайных чисел, чтобы не тянуть <random>
    unsigned int seed = 12345;
    for (int i = 0; i < count; i++) {
        seed = seed * 1103515245 + 12345;
        Heap.Insert((int)(seed % 1000000));
    }
    assert(Heap.Size() == count);

    // извлекаем всё — каждый следующий не меньше предыдущего
    int prev = Heap.ExtractMinOrMax();
    for (int i = 1; i < count; i++) {
        int cur = Heap.ExtractMinOrMax();
        assert(cur >= prev);
        prev = cur;
    }
    assert(Heap.IsEmpty());

    printf("  [OK] testBinaryHeapStress\n");
}

void testBinaryHeapAll() {
    printf("=== Тесты BinaryHeap ===\n");
    testBinaryHeapInsertExtract();
    testBinaryHeapMaxHeap();
    testBinaryHeapTop();
    testBinaryHeapDuplicates();
    testBinaryHeapEmpty();
    testBinaryHeapStress();
    printf("=== Все тесты пройдены! ===\n\n");
}
