#include <cassert> // assert
#include <cstdio> // printf, remove
#include <functional> // std::function
#include <string>

#include "../core/Streams/ParentsStream.h"
#include "../core/Streams/ReadStreams.h"
#include "../core/Streams/WriteStreams.h"
#include "../core/Lazy/LazySequence.h"
#include "../core/Sequences/MutableArraySequence.h"

void testSequenceReadStream() {
    int items[] = {10, 20, 30};
    MutableArraySequence<int> Seq(items, 3);
    SequenceReadStream<int> Stream(&Seq);
    Stream.Open();

    assert(!Stream.IsEndOfStream());
    assert(Stream.GetPosition() == 0);
    assert(Stream.Read() == 10);
    assert(Stream.Read() == 20);
    assert(Stream.GetPosition() == 2);

    // перемещение назад и повторное чтение
    assert(Stream.IsCanSeek());
    assert(Stream.IsCanGoBack());
    Stream.Seek(0);
    assert(Stream.Read() == 10);

    // дочитать до конца
    Stream.Seek(2);
    assert(Stream.Read() == 30);
    assert(Stream.IsEndOfStream());

    // чтение за концом — исключение
    bool threw = false;
    try { Stream.Read(); } catch (const EndOfStream&) { threw = true; }
    assert(threw);

    Stream.Close();
    printf("  [OK] testSequenceReadStream\n");
}

void testLazyReadStream() {
    // бесконечный поток натуральных — никогда не кончается
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed);
    LazyReadStream<int> Stream(&Nat);
    Stream.Open();

    assert(!Stream.IsEndOfStream());
    for (int i = 0; i < 1000; i++)
        assert(Stream.Read() == i);
    assert(!Stream.IsEndOfStream()); // всё ещё не конец
    Stream.Close();

    // конечный ленивый список
    int items[] = {5, 6, 7};
    LazySequence<int> Fin(items, 3);
    LazyReadStream<int> FinStream(&Fin);
    FinStream.Open();
    assert(FinStream.Read() == 5);
    FinStream.Seek(2);
    assert(FinStream.Read() == 7);
    assert(FinStream.IsEndOfStream());
    FinStream.Close();

    printf("  [OK] testLazyReadStream\n");
}

void testStringReadStream() {
    std::function<int(const std::string&)> toInt = [](const std::string& s) {
        return std::stoi(s);
    };
    StringReadStream<int> Stream("1 2 3 42", toInt);
    Stream.Open();

    assert(Stream.Read() == 1);
    assert(Stream.Read() == 2);
    assert(Stream.Read() == 3);
    assert(Stream.Read() == 42);
    assert(Stream.IsEndOfStream());

    Stream.Close();
    printf("  [OK] testStringReadStream\n");
}

void testSequenceWriteStream() {
    SequenceWriteStream<int> Stream;
    Stream.Open();

    // возвращает новую позицию
    assert(Stream.Write(10) == 1);
    assert(Stream.Write(20) == 2);
    assert(Stream.GetPosition() == 2);

    const MutableArraySequence<int>& result = Stream.GetResult();
    assert(result.GetLength() == 2);
    assert(result.Get(0) == 10);
    assert(result.Get(1) == 20);

    Stream.Close();
    printf("  [OK] testSequenceWriteStream\n");
}

void testFileStreamRoundtrip() {
    const char* path = "/tmp/lab4_stream_test.txt";
    std::function<std::string(const int&)> toStr = [](const int& x) {
        return std::to_string(x);
    };
    std::function<int(const std::string&)> toInt = [](const std::string& s) {
        return std::stoi(s);
    };

    // записать 0..99 в файл
    FileWriteStream<int> Writer(path, toStr);
    Writer.Open();
    for (int i = 0; i < 100; i++)
        Writer.Write(i);
    Writer.Close();

    // прочитать обратно
    FileReadStream<int> Reader(path, toInt);
    Reader.Open();
    assert(!Reader.IsCanSeek()); // файл только вперёд
    for (int i = 0; i < 100; i++) {
        assert(!Reader.IsEndOfStream());
        assert(Reader.Read() == i);
    }
    assert(Reader.IsEndOfStream());
    Reader.Close();

    remove(path); // убрать временный файл
    printf("  [OK] testFileStreamRoundtrip\n");
}

void testStreamLimit() {
    // предельное значение: миллион элементов через поток в память и обратно
    const int N = 1000000;

    // запись миллиона
    SequenceWriteStream<int> Writer;
    Writer.Open();
    for (int i = 0; i < N; i++)
        Writer.Write(i);
    assert(Writer.GetPosition() == (size_t)N);
    assert(Writer.GetResult().GetLength() == N);

    // чтение миллиона
    MutableArraySequence<int> Big;
    for (int i = 0; i < N; i++)
        Big.Append(i);
    SequenceReadStream<int> Reader(&Big);
    Reader.Open();
    long long sum = 0;
    while (!Reader.IsEndOfStream())
        sum += Reader.Read();
    assert(sum == (long long)N * (N - 1) / 2);
    Reader.Close();

    printf("  [OK] testStreamLimit\n");
}

void testStreamsAll() {
    printf("=== Тесты Streams ===\n");
    testSequenceReadStream();
    testLazyReadStream();
    testStringReadStream();
    testSequenceWriteStream();
    testFileStreamRoundtrip();
    testStreamLimit();
    printf("=== Все тесты пройдены! ===\n\n");
}
