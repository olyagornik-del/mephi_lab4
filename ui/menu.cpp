#include "menu.h"

#include <iostream> // std::cout

void printBanner() {
    std::cout << "\n";
    std::cout << "   _          _      _____  \n";
    std::cout << "  | |    __ _| |__  |___ /   \n";
    std::cout << "  | |   / _` | '_ \\   |_ \\  \n";
    std::cout << "  | |__| (_| | |_) | ___) |    \n";
    std::cout << "  |_____\\__,_|_.__/ |____/    \n";
    std::cout << "\n";
    std::cout << "  Stack  ·  Queue  ·  Deque  ·  Hanoi  \n";
    std::cout << "  ──────────────────────────────────────────  \n\n";
}

void printADTMenu() {
    std::cout << "\n  ┌──────────────────────────────────────┐\n";
    std::cout << "  │  Выберите АТД:                       │\n";
    std::cout << "  ├──────────────────────────────────────┤\n";
    std::cout << "  │  1. Стек                             │\n";
    std::cout << "  │  2. Очередь                          │\n";
    std::cout << "  │  3. Дек                              │\n";
    std::cout << "  │  4. Ханойская башня                  │\n";
    std::cout << "  │  0. Выход                            │\n";
    std::cout << "  └──────────────────────────────────────┘\n";
    std::cout << "  Выбор: ";
}

void printTypeMenu() {
    std::cout << "\n  ┌─────────────────────────────┐\n";
    std::cout << "  │  Выберите тип элементов:    │\n";
    std::cout << "  ├─────────────────────────────┤\n";
    std::cout << "  │  1. int                     │\n";
    std::cout << "  │  2. double                  │\n";
    std::cout << "  │  3. bool                    │\n";
    std::cout << "  └─────────────────────────────┘\n";
    std::cout << "  Тип: ";
}

void printStackOps() {
    std::cout << "\n  ┌──────────────────────────────┐\n";
    std::cout << "  │  1. Push                     │\n";
    std::cout << "  │  2. Pop                      │\n";
    std::cout << "  │  3. Peek                     │\n";
    std::cout << "  │  4. Map     (x → x*2)        │\n";
    std::cout << "  │  5. Where   (x ≥ 0)          │\n";
    std::cout << "  │  6. Reduce  (sum)            │\n";
    std::cout << "  │  0. <-Назад                  │\n";
    std::cout << "  └──────────────────────────────┘\n";
    std::cout << "  Выбор: ";
}

void printStackOpsBool() {
    std::cout << "\n  ┌──────────────────────────────┐\n";
    std::cout << "  │  1. Push                     │\n";
    std::cout << "  │  2. Pop                      │\n";
    std::cout << "  │  3. Peek                     │\n";
    std::cout << "  │  4. Map     (NOT)            │\n";
    std::cout << "  │  5. Where   (x == true)      │\n";
    std::cout << "  │  6. Reduce  (AND)            │\n";
    std::cout << "  │  0. <-Назад                  │\n";
    std::cout << "  └──────────────────────────────┘\n";
    std::cout << "  Выбор: ";
}

void printQueueOps() {
    std::cout << "\n  ┌──────────────────────────────┐\n";
    std::cout << "  │  1. Enqueue                  │\n";
    std::cout << "  │  2. Dequeue                  │\n";
    std::cout << "  │  3. Peek                     │\n";
    std::cout << "  │  4. Map     (x → x*2)        │\n";
    std::cout << "  │  5. Where   (x ≥ 0)          │\n";
    std::cout << "  │  6. Reduce  (sum)            │\n";
    std::cout << "  │  0. <-Назад                  │\n";
    std::cout << "  └──────────────────────────────┘\n";
    std::cout << "  Выбор: ";
}

void printQueueOpsBool() {
    std::cout << "\n  ┌──────────────────────────────┐\n";
    std::cout << "  │  1. Enqueue                  │\n";
    std::cout << "  │  2. Dequeue                  │\n";
    std::cout << "  │  3. Peek                     │\n";
    std::cout << "  │  4. Map     (NOT)            │\n";
    std::cout << "  │  5. Where   (x == true)      │\n";
    std::cout << "  │  6. Reduce  (AND)            │\n";
    std::cout << "  │  0. <-Назад                  │\n";
    std::cout << "  └──────────────────────────────┘\n";
    std::cout << "  Выбор: ";
}

void printDequeOps() {
    std::cout << "\n  ┌──────────────────────────────┐\n";
    std::cout << "  │  1. PushFront                │\n";
    std::cout << "  │  2. PushBack                 │\n";
    std::cout << "  │  3. PopFront                 │\n";
    std::cout << "  │  4. PopBack                  │\n";
    std::cout << "  │  5. PeekFront                │\n";
    std::cout << "  │  6. PeekBack                 │\n";
    std::cout << "  │  7. Map     (x → x*2)        │\n";
    std::cout << "  │  8. Where   (x ≥ 0)          │\n";
    std::cout << "  │  9. Reduce  (sum)            │\n";
    std::cout << "  │  0. <-Назад                  │\n";
    std::cout << "  └──────────────────────────────┘\n";
    std::cout << "  Выбор: ";
}

void printDequeOpsBool() {
    std::cout << "\n  ┌──────────────────────────────┐\n";
    std::cout << "  │  1. PushFront                │\n";
    std::cout << "  │  2. PushBack                 │\n";
    std::cout << "  │  3. PopFront                 │\n";
    std::cout << "  │  4. PopBack                  │\n";
    std::cout << "  │  5. PeekFront                │\n";
    std::cout << "  │  6. PeekBack                 │\n";
    std::cout << "  │  7. Map     (NOT)            │\n";
    std::cout << "  │  8. Where   (x == true)      │\n";
    std::cout << "  │  9. Reduce  (AND)            │\n";
    std::cout << "  │  0. <-Назад                  │\n";
    std::cout << "  └──────────────────────────────┘\n";
    std::cout << "  Выбор: ";
}

void printHanoiPrompt() {
    std::cout << "\n  ┌──────────────────────────────────────┐\n";
    std::cout << "  │   Ханойская башня                    │\n";
    std::cout << "  ├──────────────────────────────────────┤\n";
    std::cout << "  │  Введи число дисков (1–7),           │\n";
    std::cout << "  │  стартовый стержень (0/1/2)          │\n";
    std::cout << "  │  и целевой стержень (0/1/2).         │\n";
    std::cout << "  └──────────────────────────────────────┘\n";
}
