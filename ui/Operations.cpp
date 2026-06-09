#include <iostream>
#include <exception>
#include "menu.h"
#include "scanValues.h"
#include "../atd/Stack.h"
#include "../atd/Queue.h"
#include "../atd/Deque.h"
#include "../atd/Hanoi.h"


// Обёртка над scanInt с проверкой диапазона: гоняем пока юзер не введёт число от lo до hi.
static int scanIntInRange(int lo, int hi) {
    int x;
    while (true) {
        if (!scanInt(x)) continue;             // не число — scanInt уже ругнулся, пробуем снова
        if (x >= lo && x <= hi) return x;
        std::cout << "  Введите число от " << lo << " до " << hi << ": ";
    }
}

// Имя типа для шапки экрана АТД.
template <class T> static const char* typeName();
template <> const char* typeName<int>()    { return "int"; }
template <> const char* typeName<double>() { return "double"; }
template <> const char* typeName<bool>()   { return "bool"; }

// Чтение значения типа T с повтором при невалидном вводе.
template <class T>
static void readValue(T& out) {
    std::cout << "  Значение: ";
    while (!scanValue<T>(out)) {
        std::cout << "  Значение: ";
    }
}

//  op-функции для Map/Where/Reduce
//  Шаблоны работают для int и double (одна реализация на оба типа).
//  Для bool — отдельные не-шаблонные функции с другой логикой.

template <class T> static T op_x2(const T& x)                { return x * 2; }
template <class T> static bool op_nonNeg(const T& x)            { return x >= 0; }
template <class T> static T  op_sum(const T& a, const T& b)   { return a + b; }

static bool op_not(const bool& x)                               { return !x; }
static bool op_isTrue(const bool& x)                            { return x; }
static bool op_and(const bool& a, const bool& b)                { return a && b; }


//  Stack: вспомогательные render-функции и главный цикл
// Чистит dst и заливает в него содержимое src (используется после Map/Where).
template <class T>
static void replaceStack(Stack<T>& dst, const Stack<T>* src) {
    while (!dst.IsEmpty()) dst.Pop();
    for (int i = 0; i < src->Size(); i++) dst.Push(src->Get(i));
}

// Печатает текущее состояние стека одной строкой:
//      стек:  [ 7   _   _ ]
// Видна только вершина (Peek), остальные — '_', тк контракт Stack.
template <class T>
static void renderStackState(const Stack<T>& s) {
    int n = s.Size();
    std::cout << "             top\n";
    std::cout << "              v\n";
    std::cout << "     стек:  [ ";
    if (n == 0) {
        std::cout << "(пусто)";
    } else {
        std::cout << s.Peek();
        for (int i = 1; i < n; i++) std::cout << "   _";
    }
    std::cout << " ]\n";
}

// Специализация для bool — печатает T/F вместо 1/0.
template <>
void renderStackState<bool>(const Stack<bool>& s) {
    int n = s.Size();
    std::cout << "             top\n";
    std::cout << "              v\n";
    std::cout << "     стек:  [ ";
    if (n == 0) {
        std::cout << "(пусто)";
    } else {
        std::cout << (s.Peek() ? "T" : "F");
        for (int i = 1; i < n; i++) std::cout << "   _";
    }
    std::cout << " ]\n";
}

// Главный цикл меню стека для int/double.
template <class T>
static void runStackMenuTyped() {
    Stack<T> stack;
    while (true) {
        std::cout << "\n  ─── Стек (" << typeName<T>()
                  << ")  ·  размер: " << stack.Size() << " ───\n\n";
        renderStackState(stack);
        printStackOps();
        int c = scanIntInRange(0, 6);
        if (c == 0) return;
        if (c == 1) {
            T v;
            readValue(v);
            stack.Push(v);
            std::cout << "  Push " << v << "\n";
        }
        else if (c == 2) {
            try {
                T v = stack.Pop();
                std::cout << "  Pop -> " << v << "\n";
            } catch (const std::exception& e) {
                std::cout << "  Pop: " << e.what() << "\n";
            }
        }
        else if (c == 3) {
            if (stack.IsEmpty()) std::cout << "  Peek: стек пуст\n";
            else                 std::cout << "  Peek -> " << stack.Peek() << "\n";
        }
        else if (c == 4) {
            Stack<T>* r = stack.Map(op_x2<T>);
            replaceStack(stack, r);
            delete r;
            std::cout << "  Map: каждое x -> x*2\n";
        }
        else if (c == 5) {
            Stack<T>* r = stack.Where(op_nonNeg<T>);
            replaceStack(stack, r);
            delete r;
            std::cout << "  Where: оставлены x ≥ 0\n";
        }
        else if (c == 6) {
            T s = stack.Reduce(op_sum<T>, T{});
            std::cout << "  Reduce(sum) = " << s << "\n";
        }
    }
}

// Специализация для bool: Map=NOT, Where=isTrue, Reduce=AND.
template <>
void runStackMenuTyped<bool>() {
    Stack<bool> stack;
    while (true) {
        std::cout << "\n  ─── Стек (bool)  ·  размер: " << stack.Size() << " ───\n\n";
        renderStackState(stack);
        printStackOpsBool();
        int c = scanIntInRange(0, 6);
        if (c == 0) return;
        if (c == 1) {
            bool v;
            readValue(v);
            stack.Push(v);
            std::cout << "  Push " << (v ? "T" : "F") << "\n";
        }
        else if (c == 2) {
            try {
                bool v = stack.Pop();
                std::cout << "  Pop -> " << (v ? "T" : "F") << "\n";
            } catch (const std::exception& e) {
                std::cout << "  Pop: " << e.what() << "\n";
            }
        }
        else if (c == 3) {
            if (stack.IsEmpty()) std::cout << "  Peek: стек пуст\n";
            else                 std::cout << "  Peek -> " << (stack.Peek() ? "T" : "F") << "\n";
        }
        else if (c == 4) {
            Stack<bool>* r = stack.Map(op_not);
            replaceStack(stack, r);
            delete r;
            std::cout << "  Map: каждое x -> NOT x\n";
        }
        else if (c == 5) {
            Stack<bool>* r = stack.Where(op_isTrue);
            replaceStack(stack, r);
            delete r;
            std::cout << "  Where: оставлены x == true\n";
        }
        else if (c == 6) {
            bool s = stack.Reduce(op_and, true);
            std::cout << "  Reduce(AND) = " << (s ? "T" : "F") << "\n";
        }
    }
}

// Развилка по типу — нешаблонная, тк типа ещё нет на момент выбора.
static void runStackMenu() {
    printTypeMenu();
    int t = scanIntInRange(1, 3);
    if  (t == 1) runStackMenuTyped<int>();
    else if (t == 2) runStackMenuTyped<double>();
    else runStackMenuTyped<bool>();
}


//  Queue: вспомогательные render-функции и главный цикл
template <class T>
static void replaceQueue(Queue<T>& dst, const Queue<T>* src) {
    while (!dst.IsEmpty()) dst.Dequeue();
    for (int i = 0; i < src->Size(); i++) dst.Enqueue(src->Get(i));
}

// Видна только голова (Peek/Get(0)), хвост и середина — '_'.
template <class T>
static void renderQueueState(const Queue<T>& q) {
    int n = q.Size();
    std::cout << "               front\n";
    std::cout << "                 v\n";
    std::cout << "     очередь:  [ ";
    if (n == 0) {
        std::cout << "(пусто)";
    } else {
        std::cout << q.Peek();
        for (int i = 1; i < n; i++) std::cout << "   _";
    }
    std::cout << " ]\n";
}

template <>
void renderQueueState<bool>(const Queue<bool>& q) {
    int n = q.Size();
    std::cout << "               front\n";
    std::cout << "                 v\n";
    std::cout << "     очередь:  [ ";
    if (n == 0) {
        std::cout << "(пусто)";
    } else {
        std::cout << (q.Peek() ? "T" : "F");
        for (int i = 1; i < n; i++) std::cout << "   _";
    }
    std::cout << " ]\n";
}

template <class T>
static void runQueueMenuTyped() {
    Queue<T> q;
    while (true) {
        std::cout << "\n  ─── Очередь (" << typeName<T>()
                  << ")  ·  размер: " << q.Size() << " ───\n\n";
        renderQueueState(q);
        printQueueOps();
        int c = scanIntInRange(0, 6);
        if (c == 0) return;
        if (c == 1) {
            T v;
            readValue(v);
            q.Enqueue(v);
            std::cout << "  Enqueue " << v << "\n";
        }
        else if (c == 2) {
            try {
                T v = q.Dequeue();
                std::cout << "  Dequeue -> " << v << "\n";
            } catch (const std::exception& e) {
                std::cout << "  Dequeue: " << e.what() << "\n";
            }
        }
        else if (c == 3) {
            if (q.IsEmpty()) std::cout << "  Peek: очередь пуста\n";
            else             std::cout << "  Peek -> " << q.Peek() << "\n";
        }
        else if (c == 4) {
            Queue<T>* r = q.Map(op_x2<T>);
            replaceQueue(q, r);
            delete r;
            std::cout << "  Map: каждое x -> x*2\n";
        }
        else if (c == 5) {
            Queue<T>* r = q.Where(op_nonNeg<T>);
            replaceQueue(q, r);
            delete r;
            std::cout << "  Where: оставлены x ≥ 0\n";
        }
        else if (c == 6) {
            T s = q.Reduce(op_sum<T>, T{});
            std::cout << "  Reduce(sum) = " << s << "\n";
        }
    }
}

template <>
void runQueueMenuTyped<bool>() {
    Queue<bool> q;
    while (true) {
        std::cout << "\n  ─── Очередь (bool)  ·  размер: " << q.Size() << " ───\n\n";
        renderQueueState(q);
        printQueueOpsBool();
        int c = scanIntInRange(0, 6);
        if (c == 0) return;
        if (c == 1) {
            bool v;
            readValue(v);
            q.Enqueue(v);
            std::cout << "  Enqueue " << (v ? "T" : "F") << "\n";
        }
        else if (c == 2) {
            try {
                bool v = q.Dequeue();
                std::cout << "  Dequeue -> " << (v ? "T" : "F") << "\n";
            } catch (const std::exception& e) {
                std::cout << "  Dequeue: " << e.what() << "\n";
            }
        }
        else if (c == 3) {
            if (q.IsEmpty()) std::cout << "  Peek: очередь пуста\n";
            else             std::cout << "  Peek -> " << (q.Peek() ? "T" : "F") << "\n";
        }
        else if (c == 4) {
            Queue<bool>* r = q.Map(op_not);
            replaceQueue(q, r);
            delete r;
            std::cout << "  Map: каждое x -> NOT x\n";
        }
        else if (c == 5) {
            Queue<bool>* r = q.Where(op_isTrue);
            replaceQueue(q, r);
            delete r;
            std::cout << "  Where: оставлены x == true\n";
        }
        else if (c == 6) {
            bool s = q.Reduce(op_and, true);
            std::cout << "  Reduce(AND) = " << (s ? "T" : "F") << "\n";
        }
    }
}

static void runQueueMenu() {
    printTypeMenu();
    int t = scanIntInRange(1, 3);
    if      (t == 1) runQueueMenuTyped<int>();
    else if (t == 2) runQueueMenuTyped<double>();
    else             runQueueMenuTyped<bool>();
}


//  Deque: открыт с обеих сторон -> видны и front, и back
template <class T>
static void replaceDeque(Deque<T>& dst, const Deque<T>* src) {
    while (!dst.IsEmpty()) dst.PopFront();   // PopFront — O(1)
    for (int i = 0; i < src->Size(); i++) dst.PushBack(src->Get(i));
}

template <class T>
static void renderDequeState(const Deque<T>& d) {
    int n = d.Size();
    std::cout << "     дек:  [ ";
    if (n == 0) {
        std::cout << "(пусто) ]\n";
        return;
    }
    if (n == 1) {
        std::cout << d.PeekFront() << " ]";
    } else {
        std::cout << d.PeekFront();
        for (int i = 1; i < n - 1; i++) std::cout << "   _";
        std::cout << "   " << d.PeekBack() << " ]";
    }
    std::cout << "    <- front=" << d.PeekFront();
    if (n > 1) std::cout << ",  back=" << d.PeekBack();
    std::cout << "\n";
}

template <>
void renderDequeState<bool>(const Deque<bool>& d) {
    int n = d.Size();
    std::cout << "     дек:  [ ";
    if (n == 0) {
        std::cout << "(пусто) ]\n";
        return;
    }
    const char* f = d.PeekFront() ? "T" : "F";
    if (n == 1) {
        std::cout << f << " ]    <- front=" << f << "\n";
        return;
    }
    const char* b = d.PeekBack() ? "T" : "F";
    std::cout << f;
    for (int i = 1; i < n - 1; i++) std::cout << "   _";
    std::cout << "   " << b << " ]    <- front=" << f << ",  back=" << b << "\n";
}

template <class T>
static void runDequeMenuTyped() {
    Deque<T> d;
    while (true) {
        std::cout << "\n  ─── Дек (" << typeName<T>()
                  << ")  ·  размер: " << d.Size() << " ───\n\n";
        renderDequeState(d);
        printDequeOps();
        int c = scanIntInRange(0, 9);
        if (c == 0) return;
        if (c == 1 || c == 2) {
            T v; readValue(v);
            if (c == 1) { d.PushFront(v); std::cout << "  PushFront " << v << "\n"; }
            else        { d.PushBack(v);  std::cout << "  PushBack "  << v << "\n"; }
        }
        else if (c == 3) {
            try { T v = d.PopFront(); std::cout << "  PopFront -> " << v << "\n"; }
            catch (const std::exception& e) { std::cout << "  PopFront: " << e.what() << "\n"; }
        }
        else if (c == 4) {
            try { T v = d.PopBack(); std::cout << "  PopBack -> " << v << "\n"; }
            catch (const std::exception& e) { std::cout << "  PopBack: " << e.what() << "\n"; }
        }
        else if (c == 5) {
            if (d.IsEmpty()) std::cout << "  PeekFront: дек пуст\n";
            else             std::cout << "  PeekFront -> " << d.PeekFront() << "\n";
        }
        else if (c == 6) {
            if (d.IsEmpty()) std::cout << "  PeekBack: дек пуст\n";
            else             std::cout << "  PeekBack -> " << d.PeekBack() << "\n";
        }
        else if (c == 7) {
            Deque<T>* r = d.Map(op_x2<T>);
            replaceDeque(d, r); delete r;
            std::cout << "  Map: каждое x -> x*2\n";
        }
        else if (c == 8) {
            Deque<T>* r = d.Where(op_nonNeg<T>);
            replaceDeque(d, r); delete r;
            std::cout << "  Where: оставлены x ≥ 0\n";
        }
        else if (c == 9) {
            T s = d.Reduce(op_sum<T>, T{});
            std::cout << "  Reduce(sum) = " << s << "\n";
        }
    }
}

template <>
void runDequeMenuTyped<bool>() {
    Deque<bool> d;
    while (true) {
        std::cout << "\n  ─── Дек (bool)  ·  размер: " << d.Size() << " ───\n\n";
        renderDequeState(d);
        printDequeOpsBool();
        int c = scanIntInRange(0, 9);
        if (c == 0) return;
        if (c == 1 || c == 2) {
            bool v; readValue(v);
            if (c == 1) { d.PushFront(v); std::cout << "  PushFront " << (v?"T":"F") << "\n"; }
            else        { d.PushBack(v);  std::cout << "  PushBack "  << (v?"T":"F") << "\n"; }
        }
        else if (c == 3) {
            try { bool v = d.PopFront(); std::cout << "  PopFront -> " << (v?"T":"F") << "\n"; }
            catch (const std::exception& e) { std::cout << "  PopFront: " << e.what() << "\n"; }
        }
        else if (c == 4) {
            try { bool v = d.PopBack(); std::cout << "  PopBack -> " << (v?"T":"F") << "\n"; }
            catch (const std::exception& e) { std::cout << "  PopBack: " << e.what() << "\n"; }
        }
        else if (c == 5) {
            if (d.IsEmpty()) std::cout << "  PeekFront: дек пуст\n";
            else             std::cout << "  PeekFront -> " << (d.PeekFront()?"T":"F") << "\n";
        }
        else if (c == 6) {
            if (d.IsEmpty()) std::cout << "  PeekBack: дек пуст\n";
            else             std::cout << "  PeekBack -> " << (d.PeekBack()?"T":"F") << "\n";
        }
        else if (c == 7) {
            Deque<bool>* r = d.Map(op_not);
            replaceDeque(d, r); delete r;
            std::cout << "  Map: каждое x -> NOT x\n";
        }
        else if (c == 8) {
            Deque<bool>* r = d.Where(op_isTrue);
            replaceDeque(d, r); delete r;
            std::cout << "  Where: оставлены x == true\n";
        }
        else if (c == 9) {
            bool s = d.Reduce(op_and, true);
            std::cout << "  Reduce(AND) = " << (s ? "T" : "F") << "\n";
        }
    }
}

static void runDequeMenu() {
    printTypeMenu();
    int t = scanIntInRange(1, 3);
    if      (t == 1) runDequeMenuTyped<int>();
    else if (t == 2) runDequeMenuTyped<double>();
    else             runDequeMenuTyped<bool>();
}


//  Ханойская башня: ввод N + start + target -> автопроигрывание ходов с ASCII


// Рисует один диск размером diskSize как "[[[N]]]" — без центровки, слева.
// Возвращает кол-во колонок, занятых строкой (включая хвостовой паддинг и gap).
static void renderHanoi(Stack<int> rods[3], int n) {
    const int colWidth = 2 * n + 1;   // самый большой диск
    const int gap = 3;

    // диски сверху вниз: уровень n-1 наверху, 0 у самого помоста
    for (int row = n - 1; row >= 0; row--) {
        for (int r = 0; r < 3; r++) {
            int size_on_rod = rods[r].Size();
            if (row < size_on_rod) {
                int diskSize = rods[r].Get(row);  // Get(0) = низ стержня
                for (int i = 0; i < diskSize; i++) std::cout << '[';
                std::cout << diskSize;
                for (int i = 0; i < diskSize; i++) std::cout << ']';
                int len = 2 * diskSize + 1;
                for (int i = len; i < colWidth; i++) std::cout << ' ';
            } else {
                for (int i = 0; i < colWidth; i++) std::cout << ' ';
            }
            for (int i = 0; i < gap; i++) std::cout << ' ';
        }
        std::cout << '\n';
    }

    // подложка стержней
    for (int r = 0; r < 3; r++) {
        for (int i = 0; i < colWidth; i++) std::cout << "═";
        for (int i = 0; i < gap; i++) std::cout << ' ';
    }
    std::cout << '\n';

    // номера стержней
    for (int r = 0; r < 3; r++) {
        std::cout << r;
        for (int i = 1; i < colWidth; i++) std::cout << ' ';
        for (int i = 0; i < gap; i++) std::cout << ' ';
    }
    std::cout << '\n';
}

static void runHanoi() {
    printHanoiPrompt();

    std::cout << "  Дисков (1–7): ";
    int n = scanIntInRange(1, 7);
    std::cout << "  Стартовый стержень (0/1/2): ";
    int from = scanIntInRange(0, 2);
    std::cout << "  Целевой стержень (0/1/2): ";
    int to = scanIntInRange(0, 2);

    if (from == to) {
        std::cout << "\n  Старт и цель совпадают — решать нечего.\n";
        return;
    }

    // Собираем диски: контракт Hanoi — от большего к меньшему.
    MutableArraySequence<Disk> items;
    for (int sz = n; sz >= 1; sz--) {
        Disk d = { sz, Shape::Circle, Color::Red };
        items.Append(d);
    }

    Hanoi hanoi(&items, from);
    hanoi.Solve(to);
    int moves = hanoi.MovesCount();
    std::cout << "\n  Решение в " << moves << " ходов.\n";

    // Визуальная копия для проигрывания (Stack<int> только размеры).
    Stack<int> rods[3];
    for (int sz = n; sz >= 1; sz--) rods[from].Push(sz);

    std::cout << "\n  === Начальное состояние ===\n\n";
    renderHanoi(rods, n);

    for (int i = 0; i < moves; i++) {
        Move m = hanoi.GetMove(i);
        int disk = rods[m.from].Pop();
        rods[m.to].Push(disk);
        std::cout << "\n  Ход " << (i + 1) << "/" << moves
                  << ":  стержень " << m.from << " -> " << m.to << "\n\n";
        renderHanoi(rods, n);
    }
    std::cout << "\n  Готово!\n";
}


//  Главное меню


void RunMenu() {
    printBanner();
    while (true) {
        printADTMenu();
        int c = scanIntInRange(0, 4);
        if (c == 0) {
            std::cout << "\n  Пока!\n";
            return;
        }
        if      (c == 1) runStackMenu();
        else if (c == 2) runQueueMenu();
        else if (c == 3) runDequeMenu();
        else if (c == 4) runHanoi();
    }
}
