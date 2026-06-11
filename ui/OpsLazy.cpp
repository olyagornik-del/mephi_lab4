#include <iostream>
#include <functional>
#include "menu.h"
#include "scanValues.h"
#include "sections.h"
#include "../my_except.h"
#include "../core/Lazy/Ordinal.h"
#include "../core/Lazy/LazySequence.h"
#include "../core/Sequences/MutableArraySequence.h"

// Имя типа для шапки экрана.
template <class T> static const char* typeName();
template <> const char* typeName<int>()    { return "int"; }
template <> const char* typeName<double>() { return "double"; }

// Чтение значения типа T с повтором при невалидном вводе.
template <class T>
static T readValue(const char* prompt) {
    T out;
    std::cout << prompt;
    while (!scanValue<T>(out))
        std::cout << prompt;
    return out;
}

// Печатает первые n элементов, длину-ординал и счётчик материализации.
// Если в списке несколько ω-сегментов (длина ω*k+m) — показывает начало КАЖДОГО,
// разделяя их пометкой "бесконечность спустя". Показ материализует напечатанное.
template <class T>
static void renderLazy(LazySequence<T>& seq, int n = 10) {
    Ordinal len = seq.GetLength();
    unsigned long long segs = len.GetOmegaCoeff(); // сколько бесконечных сегментов
    unsigned long long tail = len.GetFinite();     // конечный хвост после них

    std::cout << "  список: [";
    if (segs == 0) {
        // обычный конечный список — как раньше
        unsigned long long limit = tail < (unsigned long long)n ? tail : (unsigned long long)n;
        for (unsigned long long i = 0; i < limit; i++) {
            if (i) std::cout << ", ";
            std::cout << seq.Get((int)i);
        }
        if (tail > limit)
            std::cout << (limit ? ", ..." : "...");
    } else {
        // начало каждого бесконечного сегмента: элементы ω*j, ω*j+1, ...
        for (unsigned long long j = 0; j < segs; j++) {
            for (unsigned long long i = 0; i < (unsigned long long)n; i++) {
                if (i) std::cout << ", ";
                std::cout << seq.GetByOrdinal(Ordinal::FromParts(j, i));
            }
            std::cout << ", ...";
            if (j + 1 < segs || tail > 0)
                std::cout << " бесконечность спустя ... ";
        }
        // конечный хвост после последней бесконечности (например после Append)
        unsigned long long limit = tail < (unsigned long long)n ? tail : (unsigned long long)n;
        for (unsigned long long i = 0; i < limit; i++) {
            if (i) std::cout << ", ";
            std::cout << seq.GetByOrdinal(Ordinal::FromParts(segs, i));
        }
        if (tail > limit)
            std::cout << ", ...";
    }
    std::cout << "]\n";
    std::cout << "  длина: " << len.ToString()
              << "   материализовано: " << seq.GetMaterializedCount() << "\n";
}

// Создание списка: вручную или по одному из заготовленных правил.
template <class T>
static LazySequence<T>* createLazy() {
    printLazyCreateMenu();
    int choice = scanIntInRange(1, 5);

    if (choice == 1) { // вручную
        std::cout << "  Сколько элементов? ";
        int n = scanIntInRange(0, 1000000);
        MutableArraySequence<T> seq;
        for (int i = 0; i < n; i++) {
            std::cout << "  [" << i << "]: ";
            T v;
            while (!scanValue<T>(v))
                std::cout << "  [" << i << "]: ";
            seq.Append(v);
        }
        return new LazySequence<T>(&seq);
    }
    if (choice == 2) { // натуральные: следующий = сколько уже есть
        MutableArraySequence<T> seed;
        std::function<T(Sequence<T>*)> rule = [](Sequence<T>* s) {
            return (T)s->GetLength();
        };
        return new LazySequence<T>(rule, &seed);
    }
    if (choice == 3) { // прогрессия: следующий = последний + d (лямбда запоминает d)
        T a = readValue<T>("  Первый член a: ");
        T d = readValue<T>("  Шаг d: ");
        T arr[1] = { a };
        MutableArraySequence<T> seed(arr, 1);
        std::function<T(Sequence<T>*)> rule = [d](Sequence<T>* s) {
            return s->GetLast() + d;
        };
        return new LazySequence<T>(rule, &seed);
    }
    if (choice == 4) { // Фибоначчи: следующий = сумма двух последних
        T arr[2] = { (T)0, (T)1 };
        MutableArraySequence<T> seed(arr, 2);
        std::function<T(Sequence<T>*)> rule = [](Sequence<T>* s) {
            int n = s->GetLength();
            return s->Get(n - 1) + s->Get(n - 2);
        };
        return new LazySequence<T>(rule, &seed);
    }
    // факториалы: следующий = последний * номер
    T arr[1] = { (T)1 };
    MutableArraySequence<T> seed(arr, 1);
    std::function<T(Sequence<T>*)> rule = [](Sequence<T>* s) {
        return s->GetLast() * (T)s->GetLength();
    };
    return new LazySequence<T>(rule, &seed);
}

// Выбор функции для Map.
template <class T>
static std::function<T(const T&)> chooseMapFunc() {
    std::cout << "  Функция: 1. x*2   2. x*x\n  Выбор: ";
    int c = scanIntInRange(1, 2);
    if (c == 1) return [](const T& x) { return x * (T)2; };
    return [](const T& x) { return x * x; };
}

// Выбор предиката для Where — свой набор на каждый тип.
template <class T> static std::function<bool(const T&)> choosePredicate();
template <>
std::function<bool(const int&)> choosePredicate<int>() {
    std::cout << "  Предикат: 1. чётные   2. больше 10\n  Выбор: ";
    int c = scanIntInRange(1, 2);
    if (c == 1) return [](const int& x) { return x % 2 == 0; };
    return [](const int& x) { return x > 10; };
}
template <>
std::function<bool(const double&)> choosePredicate<double>() {
    std::cout << "  Предикат: 1. положительные   2. больше 10\n  Выбор: ";
    int c = scanIntInRange(1, 2);
    if (c == 1) return [](const double& x) { return x > 0; };
    return [](const double& x) { return x > 10; };
}

// Склейка с последовательностью другого типа: int поднимается до double
// ленивым Map (без материализации!), результат всегда double.
static void mixedConcat(LazySequence<int>& cur) {
    std::cout << "  Второй список будет типа double.\n";
    LazySequence<double>* other = createLazy<double>();
    std::function<double(const int&)> toDouble = [](const int& x) { return (double)x; };
    LazySequence<double>* lifted = cur.Map<double>(toDouble); // int -> double лениво
    LazySequence<double>* result = lifted->Concat(other);
    std::cout << "  Результат (тип double):\n";
    renderLazy(*result);
    delete other;
    delete lifted;
    delete result;
}
static void mixedConcat(LazySequence<double>& cur) {
    std::cout << "  Второй список будет типа int.\n";
    LazySequence<int>* other = createLazy<int>();
    std::function<double(const int&)> toDouble = [](const int& x) { return (double)x; };
    LazySequence<double>* lifted = other->Map<double>(toDouble); // int -> double лениво
    LazySequence<double>* result = cur.Concat(lifted);
    std::cout << "  Результат (тип double):\n";
    renderLazy(*result);
    delete other;
    delete lifted;
    delete result;
}

// Чтение ординала: два числа k n (подсказка печатается снаружи).
static Ordinal readOrdinal(const char* name) {
    std::cout << "  " << name << " (k n): ";
    int k = scanIntInRange(0, 1000000);
    int n = scanIntInRange(0, 1000000);
    return Ordinal::FromParts((unsigned long long)k, (unsigned long long)n);
}

// Песочница: держим текущий список, операции создают новый — старый удаляем.
template <class T>
static void runLazySandbox() {
    std::cout << "\n  ── LazySequence<" << typeName<T>() << "> ──\n";
    LazySequence<T>* cur = createLazy<T>();

    while (true) {
        std::cout << "\n";
        renderLazy(*cur);
        printLazyOps();
        int op = scanIntInRange(0, 13);
        if (op == 0) break;

        try {
            switch (op) {
                case 1: { // показать первые N
                    std::cout << "  N: ";
                    int n = scanIntInRange(1, 1000);
                    renderLazy(*cur, n);
                    break;
                }
                case 2: { // Get(i)
                    std::cout << "  i: ";
                    int i = scanIntInRange(0, 1000000);
                    std::cout << "  элемент [" << i << "] = " << cur->Get(i) << "\n";
                    break;
                }
                case 3: { // GetByOrdinal
                    printOrdinalHelp();
                    Ordinal idx = readOrdinal("индекс");
                    // сначала вычисляем: если индекс плохой, исключение вылетит
                    // до печати и не оставит обрубок "элемент [...] ="
                    T value = cur->GetByOrdinal(idx);
                    std::cout << "  элемент [" << idx.ToString() << "] = " << value << "\n";
                    break;
                }
                case 4: { // Append
                    T v = readValue<T>("  Значение: ");
                    LazySequence<T>* nw = cur->Append(v);
                    delete cur;
                    cur = nw;
                    break;
                }
                case 5: { // Prepend
                    T v = readValue<T>("  Значение: ");
                    LazySequence<T>* nw = cur->Prepend(v);
                    delete cur;
                    cur = nw;
                    break;
                }
                case 6: { // InsertAt
                    T v = readValue<T>("  Значение: ");
                    std::cout << "  Позиция: ";
                    int i = scanIntInRange(0, 1000000);
                    LazySequence<T>* nw = cur->InsertAt(v, i);
                    delete cur;
                    cur = nw;
                    break;
                }
                case 7: { // Concat (тот же тип)
                    std::cout << "  Второй список:\n";
                    LazySequence<T>* other = createLazy<T>();
                    LazySequence<T>* nw = cur->Concat(other);
                    delete other;
                    delete cur;
                    cur = nw;
                    break;
                }
                case 8: { // Map
                    std::function<T(const T&)> f = chooseMapFunc<T>();
                    // слово template обязательно: тип cur зависит от T, и без подсказки
                    // компилятор не понимает, что Map — шаблонный метод
                    LazySequence<T>* nw = cur->template Map<T>(f);
                    delete cur;
                    cur = nw;
                    break;
                }
                case 9: { // Where
                    std::cout << "  (на бесконечном списке подходящих должно быть бесконечно много,\n"
                                 "   иначе поиск несуществующего элемента не остановится)\n";
                    std::function<bool(const T&)> p = choosePredicate<T>();
                    LazySequence<T>* nw = cur->Where(p);
                    delete cur;
                    cur = nw;
                    break;
                }
                case 10: { // GetSubsequence
                    std::cout << "  start: ";
                    int s = scanIntInRange(0, 1000000);
                    std::cout << "  end: ";
                    int e = scanIntInRange(0, 1000000);
                    LazySequence<T>* nw = cur->GetSubsequence(s, e);
                    delete cur;
                    cur = nw;
                    break;
                }
                case 11: { // Reduce — сумма
                    std::function<T(const T&, const T&)> add =
                        [](const T& a, const T& b) { return a + b; };
                    T sum = cur->template Reduce<T>(add, (T)0);
                    std::cout << "  сумма элементов = " << sum << "\n";
                    break;
                }
                case 12: // склейка с другим типом
                    mixedConcat(*cur);
                    break;
                case 13: { // пересоздать
                    delete cur;
                    cur = createLazy<T>();
                    break;
                }
            }
        } catch (const ParentForErrors& e) {
            std::cout << "  Ошибка: " << e.what() << "\n";
        }
    }
    delete cur;
}

void RunLazySection() {
    printTypeMenu();
    int t = scanIntInRange(1, 2);
    if (t == 1) runLazySandbox<int>();
    else        runLazySandbox<double>();
}

void RunOrdinalCalc() {
    printOrdinalHelp();
    while (true) {
        printOrdinalMenu();
        int c = scanIntInRange(0, 3);
        if (c == 0) return;

        try {
            if (c == 1) { // сложение — показываем оба порядка сразу
                Ordinal a = readOrdinal("a");
                Ordinal b = readOrdinal("b");
                std::cout << "  a + b = " << (a + b).ToString() << "\n";
                std::cout << "  b + a = " << (b + a).ToString() << "\n";
                if ((a + b) != (b + a))
                    std::cout << "  (порядок важен — сложение ординалов некоммутативно ;)\n";
            }
            else if (c == 2) { // сравнение
                Ordinal a = readOrdinal("a");
                Ordinal b = readOrdinal("b");
                const char* rel = (a < b) ? "<" : ((a == b) ? "==" : ">");
                std::cout << "  " << a.ToString() << " " << rel << " " << b.ToString() << "\n";
            }
            else { // заготовленное демо
                Ordinal two = Ordinal::Finite(2);
                Ordinal w = Ordinal::Omega();
                std::cout << "  2 + ω = " << (two + w).ToString() << "   (конечное поглощается)\n";
                std::cout << "  ω + 2 = " << (w + two).ToString() << "   (хвост сохраняется)\n";
                std::cout << "  ω + ω = " << (w + w).ToString() << "\n";
                std::cout << "  5 < ω: " << (Ordinal::Finite(5) < w ? "да" : "нет") << "\n";
            }
        } catch (const ParentForErrors& e) {
            std::cout << "  Ошибка: " << e.what() << "\n";
        }
    }
}
