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
        core/Ordinal.h
        core/Generator.h
        core/LazySequenceGenerators.h
        core/LazySequence.h
        atd/BinaryHeap.h
        atd/FixedQueue.h
)
target_compile_options(lab4_tests PRIVATE -fsanitize=address,undefined -g -O1)
target_link_options(lab4_tests PRIVATE -fsanitize=address,undefined)
