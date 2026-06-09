#include <cassert> // assert
#include <cstdio> //printf

#include "../core/LinkedList.h"

void testLinkedListCreate() {
    // конструктор по умолчанию
    LinkedList<int> Empty;
    assert(Empty.GetLength() == 0);

    // конструктор с массивом
    int items[] = {1, 2, 3};
    LinkedList<int> FromArray(items, 3);
    assert(FromArray.GetLength() == 3);
    assert(FromArray.Get(0) == 1);
    assert(FromArray.Get(2) == 3);

    // копирующий конструктор — независимая копия
    LinkedList<int> Copy(FromArray);
    assert(Copy.GetLength() == 3);
    assert(Copy.Get(1) == 2);

    // пустой список из массива размера 0
    LinkedList<int> EmptyFromArray(items, 0);
    assert(EmptyFromArray.GetLength() == 0);

    printf("  [OK] testLinkedListCreate\n");
}

void testLinkedListGetFirstLast() {
    int items[] = {5, 10, 15};
    LinkedList<int> List(items, 3);

    assert(List.GetFirst() == 5);
    assert(List.GetLast() == 15);

    // пустой список — исключение
    LinkedList<int> Empty;
    bool threw = false;
    try { Empty.GetFirst(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { Empty.GetLast(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testLinkedListGetFirstLast\n");
}

void testLinkedListGet() {
    int items[] = {1, 2, 3};
    LinkedList<int> List(items, 3);

    assert(List.Get(0) == 1);
    assert(List.Get(2) == 3);

    // некорректный индекс — отрицательный
    bool threw = false;
    try { List.Get(-1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // некорректный индекс — за границей
    threw = false;
    try { List.Get(3); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // пустой список
    LinkedList<int> Empty;
    threw = false;
    try { Empty.Get(0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testLinkedListGet\n");
}

void testLinkedListAppend() {
    LinkedList<int> List;
    List.Append(1);
    List.Append(2);
    List.Append(3);

    assert(List.GetLength() == 3);
    assert(List.GetFirst() == 1);
    assert(List.GetLast() == 3);

    printf("  [OK] testLinkedListAppend\n");
}

void testLinkedListPrepend() {
    LinkedList<int> List;
    List.Prepend(3);
    List.Prepend(2);
    List.Prepend(1);

    assert(List.GetLength() == 3);
    assert(List.GetFirst() == 1);
    assert(List.GetLast() == 3);

    printf("  [OK] testLinkedListPrepend\n");
}

void testLinkedListInsertAt() {
    int items[] = {1, 3};
    LinkedList<int> List(items, 2);

    // вставка в середину
    List.InsertAt(1, 2);
    assert(List.GetLength() == 3);
    assert(List.Get(1) == 2);

    // вставка в начало
    List.InsertAt(0, 0);
    assert(List.GetFirst() == 0);

    // вставка в конец
    List.InsertAt(List.GetLength(), 99);
    assert(List.GetLast() == 99);

    // некорректный индекс
    bool threw = false;
    try { List.InsertAt(-1, 0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testLinkedListInsertAt\n");
}

void testLinkedListGetSubList() {
    int items[] = {1, 2, 3, 4, 5};
    LinkedList<int> List(items, 5);

    LinkedList<int>* SubList = List.GetSubList(1, 3);
    assert(SubList->GetLength() == 3);
    assert(SubList->Get(0) == 2);
    assert(SubList->Get(2) == 4);
    delete SubList;

    // граничный случай: один элемент
    SubList = List.GetSubList(2, 2);
    assert(SubList->GetLength() == 1);
    assert(SubList->Get(0) == 3);
    delete SubList;

    // некорректные индексы
    bool threw = false;
    try { List.GetSubList(3, 1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { List.GetSubList(-1, 2); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testLinkedListGetSubList\n");
}

void testLinkedListConcat() {
    int a[] = {1, 2};
    int b[] = {3, 4};
    LinkedList<int> First(a, 2);
    LinkedList<int> Second(b, 2);

    LinkedList<int>* Concatenated = First.Concat(&Second);
    assert(Concatenated->GetLength() == 4);
    assert(Concatenated->Get(0) == 1);
    assert(Concatenated->Get(3) == 4);
    delete Concatenated;

    // склеивание с пустым
    LinkedList<int> Empty;
    Concatenated = First.Concat(&Empty);
    assert(Concatenated->GetLength() == 2);
    delete Concatenated;

    Concatenated = Empty.Concat(&Second);
    assert(Concatenated->GetLength() == 2);
    delete Concatenated;

    printf("  [OK] testLinkedListConcat\n");
}

void testLinkedListRemoveAt() {
    //пустой список
    LinkedList<int> Empty;
    bool threw = false;
    try { Empty.RemoveAt(0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // индекс вне диапазона
    int items[] = {10, 20, 30, 40, 50};
    LinkedList<int> list(items, 5);

    threw = false;
    try { list.RemoveAt(-1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { list.RemoveAt(5); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    // [10, 20, 30, 40, 50] -> [10, 20, 40, 50]
    list.RemoveAt(2);
    assert(list.GetLength() == 4);
    assert(list.Get(1) == 20);
    assert(list.Get(2) == 40);

    // [10, 20, 40, 50] -> [10, 20, 40]
    list.RemoveAt(3);
    assert(list.GetLength() == 3);
    assert(list.GetLast() == 40); // tail должен переехать на 40

    // [10,20,40,99]
    list.Append(99);
    assert(list.GetLength() == 4);
    assert(list.GetLast() == 99);
    assert(list.Get(3) == 99);

    //[10, 20, 40] -> [20, 40, 99]
    list.RemoveAt(0);
    assert(list.GetLength() == 3);
    assert(list.GetFirst() == 20);
    assert(list.Get(0) == 20);

    // [7] -> []
    LinkedList<int> single;
    single.Append(7);
    single.RemoveAt(0);
    assert(single.GetLength() == 0);

    //проверяем список пуст
    threw = false;
    try { single.GetFirst(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);
    threw = false;
    try { single.RemoveAt(0); } catch (const OutOfRange&) { threw = true; }
    assert(threw);
    single.Append(123); // [] -> [123]
    assert(single.GetLength() == 1);
    assert(single.GetFirst() == 123);
    assert(single.GetLast() == 123);

    // [1, 2, 3] -> [2, 3] -> [3] -> []
    int more[] = {1, 2, 3};
    LinkedList<int> drain(more, 3);
    drain.RemoveAt(0);
    drain.RemoveAt(0);
    drain.RemoveAt(0);
    assert(drain.GetLength() == 0);

    printf("  [OK] testLinkedListRemoveAt\n");
}

void testLinkedListAll() {
    printf("=== Тесты LinkedList ===\n");
    testLinkedListCreate();
    testLinkedListGet();
    testLinkedListGetFirstLast();
    testLinkedListAppend();
    testLinkedListPrepend();
    testLinkedListInsertAt();
    testLinkedListGetSubList();
    testLinkedListConcat();
    testLinkedListRemoveAt();
    printf("=== Все тесты пройдены! ===\n\n");
}
