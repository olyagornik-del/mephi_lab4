#include <cassert> // assert
#include <cstdio> //printf

#include "../atd/Queue.h"

static int Doubl(const int& x) { return x * 2; }
static bool IsEven(const int& x) { return x % 2 == 0; }
static int AddInts(const int& a, const int& b) { return a + b; }

static void checkEnqueueDequeuePeek() {
    Queue<int> q;
    assert(q.IsEmpty());
    assert(q.Size() == 0);

    // [] -> [1, 2, 3], front=1, back=3
    q.Enqueue(1);
    q.Enqueue(2);
    q.Enqueue(3);
    assert(q.Size() == 3);
    assert(!q.IsEmpty());

    // FIFO: Peek возвращает первого пришедшего и не удаляет
    assert(q.Peek() == 1);
    assert(q.Size() == 3);

    // FIFO: уходят в порядке прихода
    assert(q.Dequeue() == 1);
    assert(q.Dequeue() == 2);
    assert(q.Dequeue() == 3);
    assert(q.IsEmpty());

    // Dequeue/Peek пустой — исключение
    bool threw = false;
    try { q.Dequeue(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { q.Peek(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);
}

void testQueueEnqueueDequeuePeek() {
    checkEnqueueDequeuePeek();
    printf("  [OK] testQueueEnqueueDequeuePeek\n");
}

void testQueueCopy() {
    Queue<int> a;
    a.Enqueue(1); a.Enqueue(2); a.Enqueue(3);

    // копирующий конструктор — глубокая копия
    Queue<int> b(a);
    assert(b.Size() == 3);
    assert(b.Peek() == 1);

    // меняем копию — оригинал не трогается
    b.Enqueue(99);
    assert(b.Size() == 4);
    assert(a.Size() == 3);

    // меняем оригинал — копия не трогается
    a.Dequeue();
    assert(a.Size() == 2);
    assert(b.Size() == 4);

    // Clone — тоже независимая копия
    ICollection<int>* cl = a.Clone();
    assert(cl->GetCount() == 2);
    a.Dequeue();
    assert(a.Size() == 1);
    assert(cl->GetCount() == 2);
    delete cl;

    printf("  [OK] testQueueCopy\n");
}

void testQueueMapWhereReduce() {
    Queue<int> q;
    for (int i = 1; i <= 5; i++) q.Enqueue(i);

    // Doubl: [1, 2, 3, 4, 5] -> [2, 4, 6, 8, 10]
    Queue<int>* doubled = q.Map(Doubl);
    assert(doubled->Size() == 5);
    assert(doubled->Get(0) == 2);
    assert(doubled->Get(4) == 10);
    // оригинал не изменился
    assert(q.Get(0) == 1);
    delete doubled;

    // IsEven: [1, 2, 3, 4, 5] -> [2, 4]
    Queue<int>* evens = q.Where(IsEven);
    assert(evens->Size() == 2);
    assert(evens->Get(0) == 2);
    assert(evens->Get(1) == 4);
    delete evens;

    // 0 + 1 + 2 + 3 + 4 + 5 = 15
    int sum = q.Reduce(AddInts, 0);
    assert(sum == 15);
    // 10 + 1 + 2 + 3 + 4 + 5 = 25
    int sum10 = q.Reduce(AddInts, 10);
    assert(sum10 == 25);

    printf("  [OK] testQueueMapWhereReduce\n");
}

void testQueueConcat() {
    Queue<int> a; a.Enqueue(1); a.Enqueue(2);
    Queue<int> b; b.Enqueue(3); b.Enqueue(4);

    // [1, 2] + [3, 4] = [1, 2, 3, 4]
    Queue<int>* c = a.Concat(&b);
    assert(c->Size() == 4);
    assert(c->Get(0) == 1);
    assert(c->Get(3) == 4);
    // операнды не изменились
    assert(a.Size() == 2);
    assert(b.Size() == 2);
    delete c;

    // склеивание с пустым
    Queue<int> empty;
    Queue<int>* c2 = a.Concat(&empty);
    assert(c2->Size() == 2);
    delete c2;

    // nullptr — исключение
    bool threw = false;
    try { a.Concat(nullptr); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testQueueConcat\n");
}

void testQueueGetSubsequence() {
    Queue<int> q;
    for (int i = 1; i <= 5; i++) q.Enqueue(i);

    // [1, 2, 3, 4, 5] -> [2, 3, 4]
    Queue<int>* sub = q.GetSubsequence(1, 3);
    assert(sub->Size() == 3);
    assert(sub->Get(0) == 2);
    assert(sub->Get(2) == 4);
    delete sub;

    // граничный случай: один элемент [3]
    Queue<int>* one = q.GetSubsequence(2, 2);
    assert(one->Size() == 1);
    assert(one->Get(0) == 3);
    delete one;

    // некорректные индексы — исключение
    bool threw = false;
    try { q.GetSubsequence(3, 1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { q.GetSubsequence(-1, 2); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testQueueGetSubsequence\n");
}

void testQueueFindSubsequence() {
    Queue<int> q;
    for (int i = 1; i <= 5; i++) q.Enqueue(i);

    // [1, 2] в начале -> 0
    Queue<int> atStart; atStart.Enqueue(1); atStart.Enqueue(2);
    assert(q.FindSubsequence(&atStart) == 0);

    // [3, 4] в середине -> 2
    Queue<int> atMid; atMid.Enqueue(3); atMid.Enqueue(4);
    assert(q.FindSubsequence(&atMid) == 2);

    // [4, 5] в конце -> 3
    Queue<int> atEnd; atEnd.Enqueue(4); atEnd.Enqueue(5);
    assert(q.FindSubsequence(&atEnd) == 3);

    // [2, 4] не подряд -> -1
    Queue<int> notContig; notContig.Enqueue(2); notContig.Enqueue(4);
    assert(q.FindSubsequence(&notContig) == -1);

    // sub длиннее q -> -1
    Queue<int> tooLong;
    for (int i = 1; i <= 6; i++) tooLong.Enqueue(i);
    assert(q.FindSubsequence(&tooLong) == -1);

    // пустой sub — исключение (договорённость)
    Queue<int> emptyNeedle;
    bool threw = false;
    try { q.FindSubsequence(&emptyNeedle); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    // nullptr — исключение
    threw = false;
    try { q.FindSubsequence(nullptr); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testQueueFindSubsequence\n");
}

void testQueueAsICollection() {
    Queue<int>* q = new Queue<int>();
    q->Enqueue(10); q->Enqueue(20); q->Enqueue(30);

    // вверх по иерархии: Queue<T>* -> ICollection<T>*
    ICollection<int>* c = q;
    assert(c->GetCount() == 3);
    assert(c->Get(0) == 10);
    assert(c->Get(2) == 30);

    // полиморфный Clone — независимая копия
    ICollection<int>* clone = c->Clone();
    assert(clone->GetCount() == 3);
    q->Dequeue();
    assert(q->Size() == 2);
    assert(clone->GetCount() == 3);
    delete clone;

    delete q;

    // удаление через ICollection<T>* — ключевой сценарий M-1
    // без virtual ~ICollection ASan кинет new-delete-type-mismatch
    ICollection<int>* viaBase = new Queue<int>();
    delete viaBase;

    printf("  [OK] testQueueAsICollection\n");
}

void testQueueAll() {
    printf("=== Тесты Queue ===\n");
    testQueueEnqueueDequeuePeek();
    testQueueCopy();
    testQueueMapWhereReduce();
    testQueueConcat();
    testQueueGetSubsequence();
    testQueueFindSubsequence();
    testQueueAsICollection();
    printf("=== Все тесты пройдены! ===\n\n");
}
