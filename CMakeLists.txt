cmake_minimum_required(VERSION 4.0)
project(lab4)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab4 main.cpp
        atd/BinaryHeap.h
        atd/FixedQueue.h)

add_executable(lab4_tests
        tests/test.cpp
        tests/testDynamicArray.cpp
        tests/testLinkedList.cpp
        tests/testArraySequence.cpp
        tests/testListSequence.cpp
        tests/testMutable.cpp
        tests/testImmutable.cpp
        core/Sequences/BitSequence.h
        tests/testBitSequence.cpp
        my_except.h
        core/DynamicArray.h
        core/LinkedList.h
        core/Sequences/Sequence.h
        core/Sequences/MutableArraySequence.h
        core/Sequences/MutableListSequence.h
        core/Sequences/ImmutableArraySequence.h
        core/Sequences/ImmutableListSequence.h
        tests/testICollection.cpp
        tests/testStack.cpp
        tests/testQueue.cpp
        tests/testDeque.cpp
        tests/testHanoi.cpp
        tests/testBinaryHeap.cpp
        tests/testFixedQueue.cpp
        tests/testOrdinal.cpp
        tests/testLazySequence.cpp
        tests/testGenerator.cpp
        tests/testStreams.cpp
        tests/testOnlineStats.cpp
        tests/testStreamSorter.cpp
        tasks/OnlineStats.h
        tasks/StreamSorter.h
        core/Lazy/Ordinal.h
        core/Lazy/Generator.h
        core/Lazy/LazySequenceGenerators.h
        core/Lazy/LazySequence.h
        core/Streams/ParentsStream.h
        core/Streams/ReadStreams.h
        core/Streams/WriteStreams.h
        atd/BinaryHeap.h
        atd/FixedQueue.h
)
target_compile_options(lab4_tests PRIVATE -fsanitize=address,undefined -g -O1)
target_link_options(lab4_tests PRIVATE -fsanitize=address,undefined)

# Консольный UI: песочница LazySequence, ординалы, медиана, сортировка, авто-демо
add_executable(lab4_ui
        ui/main.cpp
        ui/menu.cpp
        ui/menu.h
        ui/scanValues.cpp
        ui/scanValues.h
        ui/Operations.cpp
        ui/OpsLazy.cpp
        ui/OpsStreams.cpp
        ui/sections.h
        my_except.h
        core/Lazy/Ordinal.h
        core/Lazy/Generator.h
        core/Lazy/LazySequenceGenerators.h
        core/Lazy/LazySequence.h
        core/Streams/ParentsStream.h
        core/Streams/ReadStreams.h
        core/Streams/WriteStreams.h
        tasks/OnlineStats.h
        tasks/StreamSorter.h
        atd/BinaryHeap.h
)
target_compile_options(lab4_ui PRIVATE -O2)
