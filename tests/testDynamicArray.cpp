#include <cassert> // assert
#include <cstdio> //printf

#include "../core/DynamicArray.h"

void testDynamicArrayCreate() {
    // конструктор по умолчанию
    DynamicArray<int> Default;
    assert(Default.GetSize() == 0);

    // конструктор с размером — элементы инициализируются нулём
    DynamicArray<int> WithSize(5);
    assert(WithSize.GetSize() == 5);
    for (int i = 0; i < 5; i++)
        assert(WithSize.Get(i) == 0);

    // конструктор с данными
    int items[] = {1, 2, 3};
    DynamicArray<int> WithData(items, 3);
    assert(WithData.GetSize() == 3);
    assert(WithData.Get(0) == 1);
    assert(WithData.Get(2) == 3);

    // копирующий конструктор — независимая копия
    DynamicArray<int> Copy(WithData);
    assert(Copy.GetSize() == 3);
    assert(Copy.Get(1) == 2);
    Copy.Set(0, 99);
    assert(WithData.Get(0) == 1); // оригинал не изменился

    // отрицательный размер — исключение
    bool threw = false;
    try { DynamicArray<int> bad(-1); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testDynamicArrayCreate\n");
}

void testDynamicArrayGet() {
    int items[] = {10, 20, 30};
    DynamicArray<int> Array(items, 3);

    assert(Array.Get(0) == 10);
    assert(Array.Get(2) == 30);

    // выход за границы
    bool threw = false;
    try { Array.Get(3); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testDynamicArrayGet\n");
}

void testDynamicArraySet() {
    DynamicArray<int> Array(3);
    Array.Set(0, 42);
    assert(Array.Get(0) == 42);

    // остальные элементы не изменились
    assert(Array.Get(1) == 0);

    // выход за границы
    bool threw = false;
    try { Array.Set(5, 99); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testDynamicArraySet\n");
}

void testDynamicArrayResize() {
    int items[] = {1, 2, 3};
    DynamicArray<int> Array(items, 3);

    // увеличение — старые данные сохраняются, новые = 0
    Array.Resize(5);
    assert(Array.GetSize() == 5);
    assert(Array.Get(0) == 1);
    assert(Array.Get(2) == 3);
    assert(Array.Get(4) == 0);

    // уменьшение
    Array.Resize(2);
    assert(Array.GetSize() == 2);
    assert(Array.Get(1) == 2);

    printf("  [OK] testDynamicArrayResize\n");
}

void testDynamicArrayRemoveAt() {
    DynamicArray<int> Empty;

    //передаём пустой список
    bool threw = false;
    try { Empty.RemoveAt(0); } catch (const OutOfRange) { threw = true; }
    assert(threw);

    //передаём заполненный массив и проверяем на выход за пределы
    int items[] = {1, 2, 3, 4, 5, 6};
    DynamicArray<int> Array(items, 6);
    threw = false;
    try { Array.RemoveAt(6); } catch (const OutOfRange) { threw = true; }
    assert(threw);

    //[1, 2, 3, 4, 5, 6] -> [1, 2, 4, 5, 6]
    Array.RemoveAt(2);
    assert(Array.GetSize() == 5);
    assert(Array.Get(1) == 2);
    assert(Array.Get(2) == 4);

    //[1, 2, 4, 5, 6] -> [2, 4, 5, 6]
    Array.RemoveAt(0);
    assert(Array.GetSize() == 4);
    assert(Array.Get(0) == 2);
    assert(Array.Get(1) == 4);

    //[2, 4, 5, 6] -> [2, 4, 5]
    Array.RemoveAt(3);
    assert(Array.GetSize() == 3);
    assert(Array.Get(2) == 5);


    printf("  [OK] testDynamicArrayRemoveAt\n");
}

void testDynamicArrayAll() {
    printf("=== Тесты DynamicArray ===\n");
    testDynamicArrayCreate();
    testDynamicArrayGet();
    testDynamicArraySet();
    testDynamicArrayResize();
    testDynamicArrayRemoveAt();
    printf("=== Все тесты пройдены! ===\n\n");
}
