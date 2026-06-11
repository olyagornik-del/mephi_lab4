#include <iostream>
#include <functional>
#include <string>
#include <chrono> // замер времени операций
#include <fstream> // проверка, что файл по пути вообще открывается
#include "menu.h"
#include "scanValues.h"
#include "sections.h"
#include "../my_except.h"
#include "../core/Lazy/LazySequence.h"
#include "../core/Streams/ReadStreams.h"
#include "../core/Streams/WriteStreams.h"
#include "../core/Sequences/MutableArraySequence.h"
#include "../tasks/OnlineStats.h"
#include "../tasks/StreamSorter.h"

// сериализация int для файловых потоков
static std::function<std::string(const int&)> toStr =
    [](const int& x) { return std::to_string(x); };
static std::function<int(const std::string&)> toInt =
    [](const std::string& s) -> int {
        try { return std::stoi(s); }
        catch (...) { throw InvalidArgument("в файле не целое число (или пустая строка)"); }
    };

// секундомер: засекли точку — спросили, сколько прошло
static std::chrono::steady_clock::time_point tick() {
    return std::chrono::steady_clock::now();
}
static double secondsSince(std::chrono::steady_clock::time_point t0) {
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - t0).count();
}

// псевдослучайные данные с настраиваемыми параметрами (тот же LCG, что в тестах)
static MutableArraySequence<int>* makeRandomData(int count, int lo, int hi, unsigned int seed) {
    if (lo > hi) { int t = lo; lo = hi; hi = t; }
    MutableArraySequence<int>* data = new MutableArraySequence<int>();
    unsigned int s = seed;
    for (int i = 0; i < count; i++) {
        s = s * 1103515245 + 12345;
        data->Append(lo + (int)(s % (unsigned int)(hi - lo + 1)));
    }
    return data;
}

// чтение целого с повтором при невалидном вводе (диапазон не ограничиваем)
static int readInt(const char* prompt) {
    int x;
    std::cout << prompt;
    while (!scanInt(x))
        std::cout << prompt;
    return x;
}

// файл-источник: сначала требования и шанс передумать, потом путь с проверкой.
// true — путь в out и файл открывается; false — пользователь передумал или файла нет
static bool askFilePath(std::string& out) {
    printFileHelp();
    std::cout << "  Продолжить? 1-да 0-назад: ";
    if (scanIntInRange(0, 1) == 0)
        return false;
    std::cout << "  Путь к файлу: ";
    std::cin >> out;
    std::ifstream probe(out); // проверяем сразу, чтобы не отвечать на остальные вопросы зря
    if (!probe.is_open()) {
        std::cout << "  Файл не открылся — проверь путь.\n";
        return false;
    }
    return true;
}

// одна строка текущих статистик
static void printStatsLine(const OnlineStats<int>& stats) {
    std::cout << "  n=" << stats.GetCount()
              << "  медиана=" << stats.GetMedian()
              << "  мин=" << stats.GetMin()
              << "  макс=" << stats.GetMax()
              << "  среднее=" << stats.GetMean() << "\n";
}

// первые n элементов последовательности одной строкой
static void printPrefix(const MutableArraySequence<int>& seq, int n, const char* label) {
    std::cout << "  " << label << ": [";
    int limit = seq.GetLength() < n ? seq.GetLength() : n;
    for (int i = 0; i < limit; i++) {
        if (i) std::cout << ", ";
        std::cout << seq.Get(i);
    }
    if (seq.GetLength() > limit) std::cout << ", ...";
    std::cout << "]\n";
}

//  Раздел 3: онлайн-статистика

void RunStatsSection() {
    while (true) {
        printStatsMenu();
        int c = scanIntInRange(0, 4);
        if (c == 0) return;

        try {
            if (c == 1) { // ручной режим: число за числом, после каждого — статистики
                std::cout << "  Вводи числа (q — закончить):\n";
                OnlineStats<int> stats;
                std::string word;
                while (std::cin >> word) {
                    if (word == "q") break;
                    int x;
                    try { x = std::stoi(word); }
                    catch (...) { std::cout << "  не число, попробуй ещё (q — выход)\n"; continue; }
                    stats.Add(x);
                    printStatsLine(stats);
                }
            }
            else if (c == 2) { // случайные данные с параметрами
                std::cout << "  Сколько элементов: ";
                int count = scanIntInRange(1, 10000000);
                int lo = readInt("  Минимум: ");
                int hi = readInt("  Максимум: ");
                std::cout << "  Зерно случайности (одно и то же зерно даёт одинаковый набор чисел): ";
                int seed = scanIntInRange(0, 1000000000);

                MutableArraySequence<int>* data = makeRandomData(count, lo, hi, (unsigned int)seed);
                printPrefix(*data, 10, "исходные данные");

                SequenceReadStream<int> in(data);
                in.Open();
                OnlineStats<int> stats;
                auto t0 = tick();
                stats.ConsumeAll(in);
                double sec = secondsSince(t0);
                printStatsLine(stats);
                std::cout << "  обработано за " << sec << " с\n";
                in.Close();
                delete data;
            }
            else if (c == 3) { // первые N бесконечного потока
                std::cout << "  Поток: 1. натуральные 0,1,2,...  2. Фибоначчи  3. прогрессия a, a+d,...\n";
                std::cout << "  Выбор: ";
                int kind = scanIntInRange(1, 3);
                std::cout << "  Сколько элементов взять (N): ";
                int n = scanIntInRange(1, 10000000);

                MutableArraySequence<int> seedArr;
                std::function<int(Sequence<int>*)> rule;
                if (kind == 1) { // натуральные
                    rule = [](Sequence<int>* s) { return s->GetLength(); };
                }
                else if (kind == 2) { // Фибоначчи
                    if (n > 46)
                        std::cout << "  (осторожно: после 46-го числа Фибоначчи int переполняется)\n";
                    seedArr.Append(0);
                    seedArr.Append(1);
                    rule = [](Sequence<int>* s) {
                        int len = s->GetLength();
                        return s->Get(len - 1) + s->Get(len - 2);
                    };
                }
                else { // прогрессия
                    int a = readInt("  Первый член a: ");
                    int d = readInt("  Шаг d: ");
                    seedArr.Append(a);
                    rule = [d](Sequence<int>* s) { return s->GetLast() + d; };
                }
                LazySequence<int> seq(rule, &seedArr);

                // показать, что именно потечёт в статистику
                int show = n < 20 ? n : 20;
                std::cout << "  начало потока: [";
                for (int i = 0; i < show; i++) {
                    if (i) std::cout << ", ";
                    std::cout << seq.Get(i);
                }
                std::cout << ", ...]\n";

                LazyReadStream<int> in(&seq);
                in.Open();
                OnlineStats<int> stats;
                stats.ConsumeFirstN(in, (size_t)n);
                printStatsLine(stats);
                in.Close();
            }
            else { // из файла
                std::string path;
                if (!askFilePath(path)) continue; // передумал или файла нет — назад в меню
                FileReadStream<int> in(path, toInt);
                in.Open();
                OnlineStats<int> stats;
                stats.ConsumeAll(in);
                printStatsLine(stats);
                in.Close();
            }
        } catch (const std::bad_alloc&) {
            std::cout << "  Ошибка: не хватило памяти.\n"
                         "  Точной медиане нужны все прочитанные числа — возьми меньше элементов.\n";
        } catch (const ParentForErrors& e) {
            std::cout << "  Ошибка: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "  Ошибка: " << e.what() << "\n";
        }
    }
}

//  Раздел 4: сортировка потока

// собственно сортировка с замером времени; направление и режим выбраны заранее
static void doSort(ReadOnlyStream<int>& in, SequenceWriteStream<int>& out,
                   bool ascending, bool inMemory, int chunkSize) {
    auto t0 = tick();
    if (inMemory) {
        if (ascending) StreamSorter<int>::SortInMemory(in, out);
        else           StreamSorter<int, std::greater<int>>::SortInMemory(in, out);
    } else {
        if (ascending) StreamSorter<int>::SortExternal(in, out, (size_t)chunkSize, toStr, toInt);
        else           StreamSorter<int, std::greater<int>>::SortExternal(in, out, (size_t)chunkSize, toStr, toInt);
    }
    double sec = secondsSince(t0);

    const MutableArraySequence<int>& result = out.GetResult();
    int n = result.GetLength();
    std::cout << "  отсортировано " << n << " элементов за " << sec << " с\n";
    printPrefix(result, 10, "первые 10");
    if (n > 10) {
        int from = n - 10;
        if (from < 10) from = 10; // не повторяем то, что уже показали в первых 10
        std::cout << "  последние 10: [";
        for (int i = from; i < n; i++) {
            if (i > from) std::cout << ", ";
            std::cout << result.Get(i);
        }
        std::cout << "]\n";
    }

    std::cout << "  Сохранить результат в файл? 1-да 0-нет: ";
    while (scanIntInRange(0, 1) == 1) {
        std::cout << "  Путь (из терминала: data/result.txt; из CLion: ../data/result.txt): ";
        std::string path;
        std::cin >> path;
        // запись в своём try: при плохом пути результат сортировки не теряем,
        // а даём ввести другой путь
        try {
            FileWriteStream<int> writer(path, toStr);
            writer.Open();
            for (int i = 0; i < n; i++)
                writer.Write(result.Get(i));
            writer.Close();
            std::cout << "  записано в " << path << "\n";
            break;
        } catch (const ParentForErrors& e) {
            std::cout << "  Не записалось: " << e.what() << "\n";
            std::cout << "  (все папки в пути должны существовать; перезаписывать существующий файл можно)\n";
            std::cout << "  Попробовать другой путь? 1-да 0-нет: ";
        }
    }
}

void RunSortSection() {
    while (true) {
        printSortSourceMenu();
        int src = scanIntInRange(0, 3);
        if (src == 0) return;

        // источник собираем ПЕРВЫМ: если файл не подходит или передумал —
        // возвращаемся в меню, не отвечая на остальные вопросы зря
        MutableArraySequence<int> manualData;
        MutableArraySequence<int>* randomData = nullptr;
        std::string path;

        if (src == 1) { // вручную
            std::cout << "  Сколько элементов? ";
            int n = scanIntInRange(0, 100000);
            for (int i = 0; i < n; i++) {
                std::cout << "  [" << i << "]: ";
                int v;
                while (!scanInt(v))
                    std::cout << "  [" << i << "]: ";
                manualData.Append(v);
            }
        }
        else if (src == 2) { // случайные
            std::cout << "  Сколько элементов: ";
            int count = scanIntInRange(1, 10000000);
            int lo = readInt("  Минимум: ");
            int hi = readInt("  Максимум: ");
            std::cout << "  Зерно случайности (одно и то же зерно даёт одинаковый набор чисел): ";
            int seed = scanIntInRange(0, 1000000000);
            randomData = makeRandomData(count, lo, hi, (unsigned int)seed);
            printPrefix(*randomData, 10, "исходные данные");
        }
        else { // файл: требования, шанс передумать, путь с проверкой
            if (!askFilePath(path)) continue;
        }

        // параметры сортировки — когда источник уже точно есть
        std::cout << "  Направление: 1. по возрастанию  2. по убыванию: ";
        bool ascending = scanIntInRange(1, 2) == 1;
        std::cout << "  Режим: 1. in-memory (всё в память — быстрее)\n";
        std::cout << "         2. external  (кусками через временные файлы — для данных больше ОЗУ)\n";
        std::cout << "  (память in-memory: ~8-16 байт на число со служебными копиями;\n";
        std::cout << "   сотни миллионов чисел — уже гигабайты ОЗУ, тогда только external.\n";
        std::cout << "   если памяти не хватит — будет ошибка, всё сделанное сотрётся\n";
        std::cout << "   и сортировку придётся начать заново)\n";
        std::cout << "  Выбор: ";
        bool inMemory = scanIntInRange(1, 2) == 1;
        int chunkSize = 0;
        if (!inMemory) {
            std::cout << "  Размер куска (chunkSize): ";
            chunkSize = scanIntInRange(1, 10000000);
        }

        try {
            SequenceWriteStream<int> out;
            out.Open();
            if (src == 1) {
                SequenceReadStream<int> in(&manualData);
                in.Open();
                doSort(in, out, ascending, inMemory, chunkSize);
                in.Close();
            }
            else if (src == 2) {
                SequenceReadStream<int> in(randomData);
                in.Open();
                doSort(in, out, ascending, inMemory, chunkSize);
                in.Close();
            }
            else {
                FileReadStream<int> in(path, toInt);
                in.Open();
                doSort(in, out, ascending, inMemory, chunkSize);
                in.Close();
            }
            out.Close();
        } catch (const std::bad_alloc&) {
            std::cout << "  Ошибка: не хватило памяти под все элементы.\n"
                         "  Возьми режим external (он держит в ОЗУ только один кусок) или меньше данных.\n";
        } catch (const ParentForErrors& e) {
            std::cout << "  Ошибка: " << e.what() << "\n";
        } catch (const std::exception& e) {
            std::cout << "  Ошибка: " << e.what() << "\n";
        }
        delete randomData; // и при исключении тоже дойдём сюда
    }
}

//  Раздел 5: авто-демо — прогон заготовленных сценариев без ввода

void RunAutoDemo() {
    std::cout << "\n  ════ Авто-демо: заготовленные сценарии ════\n";

    // 1. бесконечный Фибоначчи и ленивость
    {
        std::cout << "\n  [1] Бесконечный Фибоначчи\n";
        MutableArraySequence<int> seedArr;
        seedArr.Append(0);
        seedArr.Append(1);
        std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
            int n = s->GetLength();
            return s->Get(n - 1) + s->Get(n - 2);
        };
        LazySequence<int> fib(rule, &seedArr);
        std::cout << "  первые 10: ";
        for (int i = 0; i < 10; i++) std::cout << fib.Get(i) << " ";
        std::cout << "\n  материализовано: " << fib.GetMaterializedCount();
        std::cout << "\n  Get(30) = " << fib.Get(30)
                  << ", материализовано стало: " << fib.GetMaterializedCount() << "\n";
    }

    // 2. конкат двух бесконечных и прыжок через ω
    {
        std::cout << "\n  [2] Concat двух бесконечных + прыжок через ω\n";
        MutableArraySequence<int> seedA;
        std::function<int(Sequence<int>*)> ruleA = [](Sequence<int>* s) {
            return s->GetLength();
        };
        LazySequence<int> a(ruleA, &seedA); // 0,1,2,...

        MutableArraySequence<int> seedB;
        seedB.Append(100);
        std::function<int(Sequence<int>*)> ruleB = [](Sequence<int>* s) {
            return s->GetLast() + 1;
        };
        LazySequence<int> b(ruleB, &seedB); // 100,101,...

        LazySequence<int>* c = a.Concat(&b);
        std::cout << "  длина A=ω, длина B=ω, длина A++B = " << c->GetLength().ToString() << "\n";
        std::cout << "  Get(5) = " << c->Get(5) << "  (из A)\n";
        std::cout << "  GetByOrdinal(ω)   = " << c->GetByOrdinal(Ordinal::Omega()) << "  (первый из B!)\n";
        std::cout << "  GetByOrdinal(ω+3) = " << c->GetByOrdinal(Ordinal::FromParts(1, 3)) << "\n";
        delete c;
    }

    // 3. алгебра ординалов
    {
        std::cout << "\n  [3] Алгебра ординалов\n";
        Ordinal two = Ordinal::Finite(2);
        Ordinal w = Ordinal::Omega();
        std::cout << "  2 + ω = " << (two + w).ToString()
                  << ",  ω + 2 = " << (w + two).ToString()
                  << "  → сложение некоммутативно\n";
        std::cout << "  ω + ω = " << (w + w).ToString()
                  << ",  5 < ω: " << (Ordinal::Finite(5) < w ? "да" : "нет") << "\n";
    }

    // 4. склейка int + double через ленивый Map
    {
        std::cout << "\n  [4] Склейка списков разных типов (int ++ double)\n";
        int ai[3] = {1, 2, 3};
        LazySequence<int> a(ai, 3);
        double bd[2] = {0.5, 1.5};
        LazySequence<double> b(bd, 2);
        std::function<double(const int&)> toD = [](const int& x) { return (double)x; };
        LazySequence<double>* lifted = a.Map<double>(toD);
        LazySequence<double>* c = lifted->Concat(&b);
        std::cout << "  [1, 2, 3](int) ++ [0.5, 1.5](double) = [";
        for (int i = 0; i < 5; i++) {
            if (i) std::cout << ", ";
            std::cout << c->Get(i);
        }
        std::cout << "] (double)\n";
        delete lifted;
        delete c;
    }

    // 5. онлайн-медиана: 10001 элемент бесконечного потока
    {
        std::cout << "\n  [5] Онлайн-статистика: первые 10001 из бесконечных натуральных\n";
        MutableArraySequence<int> seedArr;
        std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
            return s->GetLength();
        };
        LazySequence<int> nat(rule, &seedArr);
        LazyReadStream<int> in(&nat);
        in.Open();
        OnlineStats<int> stats;
        auto t0 = tick();
        stats.ConsumeFirstN(in, 10001);
        double sec = secondsSince(t0);
        printStatsLine(stats);
        std::cout << "  за " << sec << " с; поток конечен? "
                  << (in.IsFinite() ? "да" : "нет") << "\n";
        in.Close();
    }

    // 6. сортировка: in-memory против external на одних данных
    {
        std::cout << "\n  [6] Сортировка 200000 случайных: in-memory vs external\n";
        MutableArraySequence<int>* data = makeRandomData(200000, 0, 1000000, 123);

        SequenceReadStream<int> in1(data);
        in1.Open();
        SequenceWriteStream<int> out1;
        auto t1 = tick();
        StreamSorter<int>::SortInMemory(in1, out1);
        double sec1 = secondsSince(t1);
        in1.Close();

        SequenceReadStream<int> in2(data);
        in2.Open();
        SequenceWriteStream<int> out2;
        auto t2 = tick();
        StreamSorter<int>::SortExternal(in2, out2, 20000, toStr, toInt);
        double sec2 = secondsSince(t2);
        in2.Close();

        bool same = out1.GetResult().GetLength() == out2.GetResult().GetLength();
        for (int i = 0; same && i < out1.GetResult().GetLength(); i++)
            if (out1.GetResult().Get(i) != out2.GetResult().Get(i))
                same = false;

        std::cout << "  in-memory: " << sec1 << " с;  external (chunk=20000, 10 файлов): "
                  << sec2 << " с\n";
        std::cout << "  результаты совпадают: " << (same ? "да" : "нет (баг!)") << "\n";
        delete data;
    }

    std::cout << "\n  ════ авто-демо завершено ════\n";
}
