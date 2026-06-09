#include <cassert> // assert
#include <cstdio> //printf

#include "../atd/Deque.h"

static int Doubl(const int& x) { return x * 2; }
static bool IsEven(const int& x) { return x % 2 == 0; }
static int AddInts(const int& a, const int& b) { return a + b; }

static void checkPushPopPeek() {
    Deque<int> d;
    assert(d.IsEmpty());
    assert(d.Size() == 0);

    // [] -> [2] -> [2, 3] -> [1, 2, 3]
    d.PushBack(2);
    d.PushBack(3);
    d.PushFront(1);
    assert(d.Size() == 3);
    assert(!d.IsEmpty());

    // оба конца видны, Peek не удаляет
    assert(d.PeekFront() == 1);
    assert(d.PeekBack() == 3);
    assert(d.Size() == 3);

    // [1, 2, 3] -> [0, 1, 2, 3] -> [0, 1, 2, 3, 4]
    d.PushFront(0);
    d.PushBack(4);
    assert(d.PeekFront() == 0);
    assert(d.PeekBack() == 4);
    assert(d.Size() == 5);

    // [0, 1, 2, 3, 4] -> [1, 2, 3, 4] -> [1, 2, 3]
    assert(d.PopFront() == 0);
    assert(d.PopBack() == 4);
    assert(d.PeekFront() == 1);
    assert(d.PeekBack() == 3);

    // вычёрпываем до конца с обеих сторон
    assert(d.PopFront() == 1);
    assert(d.PopBack() == 3);
    assert(d.PopBack() == 2);
    assert(d.IsEmpty());

    // Pop/Peek пустого — исключение
    bool threw = false;
    try { d.PopFront(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { d.PopBack(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { d.PeekFront(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { d.PeekBack(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);
}

void testDequePushPopPeek() {
    checkPushPopPeek();
    printf("  [OK] testDequePushPopPeek\n");
}

void testDequeCopy() {
    Deque<int> a;
    a.PushBack(1); a.PushBack(2); a.PushBack(3);

    // копирующий конструктор — глубокая копия
    Deque<int> b(a);
    assert(b.Size() == 3);
    assert(b.PeekFront() == 1);
    assert(b.PeekBack() == 3);

    // меняем копию — оригинал не трогается
    b.PushBack(99);
    assert(b.Size() == 4);
    assert(a.Size() == 3);

    // меняем оригинал — копия не трогается
    a.PopFront();
    assert(a.Size() == 2);
    assert(b.Size() == 4);

    // Clone — тоже независимая копия
    ICollection<int>* cl = a.Clone();
    assert(cl->GetCount() == 2);
    a.PopBack();
    assert(a.Size() == 1);
    assert(cl->GetCount() == 2);
    delete cl;

    printf("  [OK] testDequeCopy\n");
}

void testDequeMapWhereReduce() {
    Deque<int> d;
    for (int i = 1; i <= 5; i++) d.PushBack(i);

    // Doubl: [1, 2, 3, 4, 5] -> [2, 4, 6, 8, 10]
    Deque<int>* doubled = d.Map(Doubl);
    assert(doubled->Size() == 5);
    assert(doubled->Get(0) == 2);
    assert(doubled->Get(4) == 10);
    // оригинал не изменился
    assert(d.Get(0) == 1);
    delete doubled;

    // IsEven: [1, 2, 3, 4, 5] -> [2, 4]
    Deque<int>* evens = d.Where(IsEven);
    assert(evens->Size() == 2);
    assert(evens->Get(0) == 2);
    assert(evens->Get(1) == 4);
    delete evens;

    // 0 + 1 + 2 + 3 + 4 + 5 = 15
    int sum = d.Reduce(AddInts, 0);
    assert(sum == 15);
    // 10 + 1 + 2 + 3 + 4 + 5 = 25
    int sum10 = d.Reduce(AddInts, 10);
    assert(sum10 == 25);

    printf("  [OK] testDequeMapWhereReduce\n");
}

void testDequeConcat() {
    Deque<int> a; a.PushBack(1); a.PushBack(2);
    Deque<int> b; b.PushBack(3); b.PushBack(4);

    // [1, 2] + [3, 4] = [1, 2, 3, 4]
    Deque<int>* c = a.Concat(&b);
    assert(c->Size() == 4);
    assert(c->Get(0) == 1);
    assert(c->Get(3) == 4);
    // операнды не изменились
    assert(a.Size() == 2);
    assert(b.Size() == 2);
    delete c;

    // склеивание с пустым
    Deque<int> empty;
    Deque<int>* c2 = a.Concat(&empty);
    assert(c2->Size() == 2);
    delete c2;

    // nullptr — исключение
    bool threw = false;
    try { a.Concat(nullptr); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testDequeConcat\n");
}

void testDequeGetSubsequence() {
    Deque<int> d;
    for (int i = 1; i <= 5; i++) d.PushBack(i);

    // [1, 2, 3, 4, 5] -> [2, 3, 4]
    Deque<int>* sub = d.GetSubsequence(1, 3);
    assert(sub->Size() == 3);
    assert(sub->Get(0) == 2);
    assert(sub->Get(2) == 4);
    delete sub;

    // граничный случай: один элемент [3]
    Deque<int>* one = d.GetSubsequence(2, 2);
    assert(one->Size() == 1);
    assert(one->Get(0) == 3);
    delete one;

    // некорректные индексы — исключение
    bool threw = false;
    try { d.GetSubsequence(3, 1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { d.GetSubsequence(-1, 2); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testDequeGetSubsequence\n");
}

void testDequeFindSubsequence() {
    Deque<int> d;
    for (int i = 1; i <= 5; i++) d.PushBack(i);

    // [1, 2] в начале -> 0
    Deque<int> atStart; atStart.PushBack(1); atStart.PushBack(2);
    assert(d.FindSubsequence(&atStart) == 0);

    // [3, 4] в середине -> 2
    Deque<int> atMid; atMid.PushBack(3); atMid.PushBack(4);
    assert(d.FindSubsequence(&atMid) == 2);

    // [4, 5] в конце -> 3
    Deque<int> atEnd; atEnd.PushBack(4); atEnd.PushBack(5);
    assert(d.FindSubsequence(&atEnd) == 3);

    // [2, 4] не подряд -> -1
    Deque<int> notContig; notContig.PushBack(2); notContig.PushBack(4);
    assert(d.FindSubsequence(&notContig) == -1);

    // sub длиннее d -> -1
    Deque<int> tooLong;
    for (int i = 1; i <= 6; i++) tooLong.PushBack(i);
    assert(d.FindSubsequence(&tooLong) == -1);

    // пустой sub — исключение (договорённость)
    Deque<int> emptyNeedle;
    bool threw = false;
    try { d.FindSubsequence(&emptyNeedle); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    // nullptr — исключение
    threw = false;
    try { d.FindSubsequence(nullptr); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testDequeFindSubsequence\n");
}

void testDequeAsICollection() {
    Deque<int>* d = new Deque<int>();
    d->PushBack(10); d->PushBack(20); d->PushBack(30);

    // вверх по иерархии: Deque<T>* -> ICollection<T>*
    ICollection<int>* c = d;
    assert(c->GetCount() == 3);
    assert(c->Get(0) == 10);
    assert(c->Get(2) == 30);

    // полиморфный Clone — независимая копия
    ICollection<int>* clone = c->Clone();
    assert(clone->GetCount() == 3);
    d->PopFront();
    assert(d->Size() == 2);
    assert(clone->GetCount() == 3);
    delete clone;

    delete d;

    // удаление через ICollection<T>* — ключевой сценарий M-1
    // без virtual ~ICollection ASan кинет new-delete-type-mismatch
    ICollection<int>* viaBase = new Deque<int>();
    delete viaBase;

    printf("  [OK] testDequeAsICollection\n");
}

void testDequeAll() {
    printf("=== Тесты Deque ===\n");
    testDequePushPopPeek();
    testDequeCopy();
    testDequeMapWhereReduce();
    testDequeConcat();
    testDequeGetSubsequence();
    testDequeFindSubsequence();
    testDequeAsICollection();
    printf("=== Все тесты пройдены! ===\n\n");
}
