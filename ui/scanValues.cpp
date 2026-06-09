#include "scanValues.h"

#include <iostream> // std::cin, std::cout
#include <limits>   // std::numeric_limits

void flushInput() {
    std::cin.clear(); // сбросить флаг ошибки
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // выкинуть всё до конца строки
}

bool scanInt(int &out) {
    if (std::cin >> out) return true;
    std::cout << "  Введите целое число!\n";
    flushInput();
    return false;
}

bool scanDouble(double &out) {
    if (std::cin >> out) return true;
    std::cout << "  Ошибка: введите вещественное число!\n";
    flushInput();
    return false;
}

int inputSize() {
    int n;
    std::cout << "  Введите размер или номер элемента: ";
    while (!scanInt(n)) {
        std::cout << "  Введите размер или номер элемента (целое число): ";
    }
    if (n <= 0) {
        std::cout << "  Отрицательное число не подходит. Ставлю 1 >:^)\n";
        return 1;
    }
    return n;
}

template <> bool scanValue<int>(int &out) { return scanInt(out); }
template <> bool scanValue<double>(double &out) { return scanDouble(out); }
template <> bool scanValue<bool>(bool &out) {
    int x;
    if (!scanInt(x)) return false;
    if (x != 0 && x != 1) {
        std::cout << "  Введите 0 или 1!\n";
        return false;
    }
    out = (x == 1);
    return true;
}
