#include <cassert> // assert
#include <cstdio> //printf
#include <functional> // std::function, std::greater
#include <string>

#include "../tasks/StreamSorter.h"
#include "../core/Streams/ReadStreams.h"
#include "../core/Streams/WriteStreams.h"
#include "../core/Sequences/MutableArraySequence.h"

void testSortInMemory() {
    int items[] = {5, 2, 8, 1, 9, 3};
    MutableArraySequence<int> Seq(items, 6);
    SequenceReadStream<int> In(&Seq);
    SequenceWriteStream<int> Out;
    In.Open();
    Out.Open();

    StreamSorter<int>::SortInMemory(In, Out);

    //[5, 2, 8, 1, 9, 3] -> 1 2 3 5 8 9
    const MutableArraySequence<int>& Result = Out.GetResult();
    int expected[] = {1, 2, 3, 5, 8, 9};
    assert(Result.GetLength() == 6);
    for (int i = 0; i < 6; i++)
        assert(Result.Get(i) == expected[i]);

    In.Close();
    Out.Close();
    printf("  [OK] testSortInMemory\n");
}

void testSortDescending() {
    // тот же сортировщик, но с обратным компаратором — по убыванию
    int items[] = {5, 2, 8, 1, 9, 3};
    MutableArraySequence<int> Seq(items, 6);
    SequenceReadStream<int> In(&Seq);
    SequenceWriteStream<int> Out;
    In.Open();
    Out.Open();

    StreamSorter<int, std::greater<int>>::SortInMemory(In, Out);

    //[5, 2, 8, 1, 9, 3] -> 9 8 5 3 2 1
    const MutableArraySequence<int>& Result = Out.GetResult();
    int expected[] = {9, 8, 5, 3, 2, 1};
    for (int i = 0; i < 6; i++)
        assert(Result.Get(i) == expected[i]);

    printf("  [OK] testSortDescending\n");
}

void testSortDuplicatesAndEmpty() {
    // дубликаты сохраняются
    int items[] = {4, 4, 1, 4, 1};
    MutableArraySequence<int> Seq(items, 5);
    SequenceReadStream<int> In(&Seq);
    SequenceWriteStream<int> Out;
    StreamSorter<int>::SortInMemory(In, Out);
    //[4, 4, 1, 4, 1] -> 1 1 4 4 4
    int expected[] = {1, 1, 4, 4, 4};
    for (int i = 0; i < 5; i++)
        assert(Out.GetResult().Get(i) == expected[i]);

    // пустой поток — пустой результат, без исключений
    MutableArraySequence<int> EmptySeq;
    SequenceReadStream<int> EmptyIn(&EmptySeq);
    SequenceWriteStream<int> EmptyOut;
    StreamSorter<int>::SortInMemory(EmptyIn, EmptyOut);
    assert(EmptyOut.GetResult().GetLength() == 0);

    printf("  [OK] testSortDuplicatesAndEmpty\n");
}

void testSortExternal() {
    std::function<std::string(const int&)> toStr = [](const int& x) {
        return std::to_string(x);
    };
    std::function<int(const std::string&)> toInt = [](const std::string& s) {
        return std::stoi(s);
    };

    // 100 элементов по убыванию, куски по 7 -> 15 кусков сливаются в один порядок
    MutableArraySequence<int> Seq;
    for (int i = 99; i >= 0; i--)
        Seq.Append(i);
    SequenceReadStream<int> In(&Seq);
    SequenceWriteStream<int> Out;
    In.Open();
    Out.Open();

    StreamSorter<int>::SortExternal(In, Out, 7, toStr, toInt);

    const MutableArraySequence<int>& Result = Out.GetResult();
    assert(Result.GetLength() == 100);
    for (int i = 0; i < 100; i++)
        assert(Result.Get(i) == i);

    // нулевой размер куска — исключение
    SequenceReadStream<int> In2(&Seq);
    SequenceWriteStream<int> Out2;
    bool threw = false;
    try { StreamSorter<int>::SortExternal(In2, Out2, 0, toStr, toInt); }
    catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    printf("  [OK] testSortExternal\n");
}

void testSortExternalMatchesInMemory() {
    // одни и те же данные через оба режима — результаты совпадают
    std::function<std::string(const int&)> toStr = [](const int& x) {
        return std::to_string(x);
    };
    std::function<int(const std::string&)> toInt = [](const std::string& s) {
        return std::stoi(s);
    };

    // псевдослучайные данные
    MutableArraySequence<int> Data;
    unsigned int seed = 42;
    for (int i = 0; i < 1000; i++) {
        seed = seed * 1103515245 + 12345;
        Data.Append((int)(seed % 10000));
    }

    SequenceReadStream<int> In1(&Data);
    SequenceWriteStream<int> Out1;
    StreamSorter<int>::SortInMemory(In1, Out1);

    SequenceReadStream<int> In2(&Data);
    SequenceWriteStream<int> Out2;
    StreamSorter<int>::SortExternal(In2, Out2, 64, toStr, toInt);

    assert(Out1.GetResult().GetLength() == Out2.GetResult().GetLength());
    for (int i = 0; i < 1000; i++)
        assert(Out1.GetResult().Get(i) == Out2.GetResult().Get(i));

    printf("  [OK] testSortExternalMatchesInMemory\n");
}

void testSortStress() {
    // предельное значение: 200000 элементов в памяти
    MutableArraySequence<int> Data;
    unsigned int seed = 123;
    int count = 200000;
    for (int i = 0; i < count; i++) {
        seed = seed * 1103515245 + 12345;
        Data.Append((int)(seed % 1000000));
    }
    SequenceReadStream<int> In(&Data);
    SequenceWriteStream<int> Out;
    StreamSorter<int>::SortInMemory(In, Out);

    // каждый следующий не меньше предыдущего
    const MutableArraySequence<int>& Result = Out.GetResult();
    assert(Result.GetLength() == count);
    for (int i = 1; i < count; i++)
        assert(Result.Get(i - 1) <= Result.Get(i));

    printf("  [OK] testSortStress\n");
}

void testStreamSorterAll() {
    printf("=== Тесты StreamSorter ===\n");
    testSortInMemory();
    testSortDescending();
    testSortDuplicatesAndEmpty();
    testSortExternal();
    testSortExternalMatchesInMemory();
    testSortStress();
    printf("=== Все тесты пройдены! ===\n\n");
}
