#include <cassert> // assert
#include <cstdio> //printf

#include "../atd/Stack.h"

static int Doubl(const int& x) { return x * 2; }
static bool IsEven(const int& x) { return x % 2 == 0; }
static int AddInts(const int& a, const int& b) { return a + b; }

static void checkPushPopPeek() {
    Stack<int> s;
    assert(s.IsEmpty());
    assert(s.Size() == 0);

    s.Push(1);
    s.Push(2);
    s.Push(3);                 // снизу [1,2,3], сверху 3
    assert(s.Size() == 3);
    assert(!s.IsEmpty());

    assert(s.Peek() == 3);     // вершина — последний добавленный
    assert(s.Size() == 3);     // Peek не удаляет

    assert(s.Pop() == 3);      // LIFO
    assert(s.Pop() == 2);
    assert(s.Pop() == 1);
    assert(s.IsEmpty());

    // Pop/Peek пустого — исключение
    bool threw = false;
    try { s.Pop(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { s.Peek(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);
}

void testStackPushPopPeek() {
    checkPushPopPeek();
    printf("  [OK] testStackPushPopPeek\n");
}

void testStackCopy() {
    Stack<int> a;
    a.Push(1); a.Push(2); a.Push(3);

    Stack<int> b(a);                 // копирующий конструктор — глубокая копия
    assert(b.Size() == 3);
    assert(b.Peek() == 3);

    b.Push(99);                      // меняем копию
    assert(b.Size() == 4);
    assert(a.Size() == 3);           // оригинал не изменился

    a.Pop();                         // меняем оригинал
    assert(a.Size() == 2);
    assert(b.Size() == 4);           // копия не изменилась

    // Clone — тоже независимая копия
    ICollection<int>* cl = a.Clone();
    assert(cl->GetCount() == 2);
    a.Pop();
    assert(a.Size() == 1);
    assert(cl->GetCount() == 2);     // клон не зависит от оригинала
    delete cl;

    printf("  [OK] testStackCopy\n");
}

void testStackMapWhereReduce() {
    Stack<int> s;
    for (int i = 1; i <= 5; i++) s.Push(i);   // [1,2,3,4,5]

    Stack<int>* doubled = s.Map(Doubl);        // Doubl: [1,2,3,4,5] -> [2,4,6,8,10]
    assert(doubled->Size() == 5);
    assert(doubled->Get(0) == 2);
    assert(doubled->Get(4) == 10);
    assert(s.Get(0) == 1);                     // оригинал не изменился
    delete doubled;

    Stack<int>* evens = s.Where(IsEven);       // IsEven: [1,2,3,4,5] -> [2,4]
    assert(evens->Size() == 2);
    assert(evens->Get(0) == 2);
    assert(evens->Get(1) == 4);
    delete evens;

    int sum = s.Reduce(AddInts, 0);            // сумма с начальным 0: 0+1+2+3+4+5 = 15
    assert(sum == 15);
    int sum10 = s.Reduce(AddInts, 10);         // начальное значение не ноль
    assert(sum10 == 25);

    printf("  [OK] testStackMapWhereReduce\n");
}

void testStackConcat() {
    Stack<int> a; a.Push(1); a.Push(2);
    Stack<int> b; b.Push(3); b.Push(4);

    Stack<int>* c = a.Concat(&b);              // [1,2] + [3,4] = [1,2,3,4]
    assert(c->Size() == 4);
    assert(c->Get(0) == 1);
    assert(c->Get(3) == 4);
    assert(a.Size() == 2);                     // операнды не изменились
    assert(b.Size() == 2);
    delete c;

    // склеивание с пустым
    Stack<int> empty;
    Stack<int>* c2 = a.Concat(&empty);
    assert(c2->Size() == 2);
    delete c2;

    // nullptr — исключение
    bool threw = false;
    try { a.Concat(nullptr); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testStackConcat\n");
}

void testStackGetSubsequence() {
    Stack<int> s;
    for (int i = 1; i <= 5; i++) s.Push(i);    // [1,2,3,4,5]

    Stack<int>* sub = s.GetSubsequence(1, 3);  // [2,3,4]
    assert(sub->Size() == 3);
    assert(sub->Get(0) == 2);
    assert(sub->Get(2) == 4);
    delete sub;

    // граничный случай: один элемент
    Stack<int>* one = s.GetSubsequence(2, 2);  // [3]
    assert(one->Size() == 1);
    assert(one->Get(0) == 3);
    delete one;

    // некорректные индексы
    bool threw = false;
    try { s.GetSubsequence(3, 1); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { s.GetSubsequence(-1, 2); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testStackGetSubsequence\n");
}

void testStackFindSubsequence() {
    Stack<int> s;
    for (int i = 1; i <= 5; i++) s.Push(i);    // [1,2,3,4,5]

    Stack<int> atStart; atStart.Push(1); atStart.Push(2);   // [1,2]
    assert(s.FindSubsequence(&atStart) == 0);

    Stack<int> atMid; atMid.Push(3); atMid.Push(4);         // [3,4]
    assert(s.FindSubsequence(&atMid) == 2);

    Stack<int> atEnd; atEnd.Push(4); atEnd.Push(5);         // [4,5]
    assert(s.FindSubsequence(&atEnd) == 3);

    Stack<int> notContig; notContig.Push(2); notContig.Push(4); // [2,4] — не смежны
    assert(s.FindSubsequence(&notContig) == -1);

    Stack<int> tooLong;
    for (int i = 1; i <= 6; i++) tooLong.Push(i);           // длиннее s
    assert(s.FindSubsequence(&tooLong) == -1);

    // пустой sub — исключение (договорённость)
    Stack<int> emptyNeedle;
    bool threw = false;
    try { s.FindSubsequence(&emptyNeedle); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    // nullptr — исключение
    threw = false;
    try { s.FindSubsequence(nullptr); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testStackFindSubsequence\n");
}

void testStackAsICollection() {
    Stack<int>* s = new Stack<int>();
    s->Push(10); s->Push(20); s->Push(30);

    ICollection<int>* c = s;                   // вверх по иерархии
    assert(c->GetCount() == 3);
    assert(c->Get(0) == 10);
    assert(c->Get(2) == 30);

    ICollection<int>* clone = c->Clone();      // полиморфный Clone
    assert(clone->GetCount() == 3);
    s->Pop();                                  // меняем оригинал
    assert(s->Size() == 2);
    assert(clone->GetCount() == 3);            // клон независим
    delete clone;

    delete s;

    // удаление через ICollection<T>* — ключевой сценарий M-1
    ICollection<int>* viaBase = new Stack<int>();
    delete viaBase;  // без virtual ~ICollection ASan кинет new-delete-type-mismatch

    printf("  [OK] testStackAsICollection\n");
}

void testStackAll() {
    printf("=== Тесты Stack ===\n");
    testStackPushPopPeek();
    testStackCopy();
    testStackMapWhereReduce();
    testStackConcat();
    testStackGetSubsequence();
    testStackFindSubsequence();
    testStackAsICollection();
    printf("=== Все тесты пройдены! ===\n\n");
}
