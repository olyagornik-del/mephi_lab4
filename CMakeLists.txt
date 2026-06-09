cmake_minimum_required(VERSION 4.0)
project(lab4)

set(CMAKE_CXX_STANDARD 11)

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
)
