#include <cassert> // assert
#include <cstdio> //printf
#include <functional> // std::function

#include "../tasks/OnlineStats.h"
#include "../core/Streams/ReadStreams.h"
#include "../core/Lazy/LazySequence.h"
#include "../core/Sequences/MutableArraySequence.h"

void testOnlineStatsBasic() {
    OnlineStats<int> Stats;
    int items[] = {5, 2, 8, 1, 9};
    for (int i = 0; i < 5; i++)
        Stats.Add(items[i]);

    assert(Stats.GetCount() == 5);
    assert(Stats.GetMin() == 1);
    assert(Stats.GetMax() == 9);
    assert(Stats.GetMean() == 5.0); // (5+2+8+1+9)/5
    assert(Stats.GetMedian() == 5.0); // отсортировано 1,2,5,8,9 — середина 5

    printf("  [OK] testOnlineStatsBasic\n");
}

void testOnlineStatsRunningMedian() {
    // медиана пересчитывается после каждого элемента — режим онлайн
    OnlineStats<int> Stats;
    int values[] = {5, 2, 8, 1, 9};
    double medians[] = {5.0, 3.5, 5.0, 3.5, 5.0};
    for (int i = 0; i < 5; i++) {
        Stats.Add(values[i]);
        assert(Stats.GetMedian() == medians[i]);
    }

    printf("  [OK] testOnlineStatsRunningMedian\n");
}

void testOnlineStatsEvenCount() {
    // чётное число элементов — среднее двух центральных
    OnlineStats<int> Stats;
    int items[] = {1, 2, 3, 4};
    for (int i = 0; i < 4; i++)
        Stats.Add(items[i]);
    assert(Stats.GetMedian() == 2.5);

    printf("  [OK] testOnlineStatsEvenCount\n");
}

void testOnlineStatsDuplicates() {
    //[4, 4, 1, 4, 1] -> отсортировано 1,1,4,4,4 — медиана 4
    OnlineStats<int> Stats;
    int items[] = {4, 4, 1, 4, 1};
    for (int i = 0; i < 5; i++)
        Stats.Add(items[i]);
    assert(Stats.GetMedian() == 4.0);
    assert(Stats.GetMin() == 1);
    assert(Stats.GetMax() == 4);

    printf("  [OK] testOnlineStatsDuplicates\n");
}

void testOnlineStatsEmpty() {
    // пустая статистика — все запросы бросают
    OnlineStats<int> Empty;
    assert(Empty.GetCount() == 0);

    bool threw = false;
    try { Empty.GetMedian(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { Empty.GetMin(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    threw = false;
    try { Empty.GetMean(); } catch (const OutOfRange&) { threw = true; }
    assert(threw);

    printf("  [OK] testOnlineStatsEmpty\n");
}

void testOnlineStatsConsumeStream() {
    // статистика напрямую из потока
    int items[] = {5, 2, 8, 1, 9};
    MutableArraySequence<int> Seq(items, 5);
    SequenceReadStream<int> In(&Seq);
    In.Open();

    OnlineStats<int> Stats;
    Stats.ConsumeAll(In);
    assert(Stats.GetCount() == 5);
    assert(Stats.GetMedian() == 5.0);
    assert(In.IsEndOfStream()); // поток съеден целиком

    In.Close();
    printf("  [OK] testOnlineStatsConsumeStream\n");
}

void testOnlineStatsInfiniteStream() {
    // Бесконечный поток натуральных: статистика по первым 10001 элементам (0..10000)
    MutableArraySequence<int> Seed;
    std::function<int(Sequence<int>*)> rule = [](Sequence<int>* s) {
        return s->GetLength();
    };
    LazySequence<int> Nat(rule, &Seed);
    LazyReadStream<int> In(&Nat);
    In.Open();

    OnlineStats<int> Stats;
    Stats.ConsumeFirstN(In, 10001); // ровно n элементов — поток-то не кончится
    assert(Stats.GetCount() == 10001);
    assert(Stats.GetMin() == 0);
    assert(Stats.GetMax() == 10000);
    assert(Stats.GetMedian() == 5000.0);
    assert(!In.IsEndOfStream()); // поток всё ещё не кончился
    assert(!In.IsFinite()); // и в принципе не кончится

    // прочитать бесконечный поток целиком нельзя — исключение
    OnlineStats<int> Stats2;
    bool threw = false;
    try { Stats2.ConsumeAll(In); } catch (const InvalidArgument&) { threw = true; }
    assert(threw);

    In.Close();
    printf("  [OK] testOnlineStatsInfiniteStream\n");
}

void testOnlineStatsAll() {
    printf("=== Тесты OnlineStats ===\n");
    testOnlineStatsBasic();
    testOnlineStatsRunningMedian();
    testOnlineStatsEvenCount();
    testOnlineStatsDuplicates();
    testOnlineStatsEmpty();
    testOnlineStatsConsumeStream();
    testOnlineStatsInfiniteStream();
    printf("=== Все тесты пройдены! ===\n\n");
}
