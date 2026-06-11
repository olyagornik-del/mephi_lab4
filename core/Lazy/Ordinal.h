#ifndef LAB4_ORDINAL_H
#define LAB4_ORDINAL_H

#include "../../my_except.h"
#include <string>

// порядковое число вида ω·omega_coeff + finite
// omega_coeff == 0 -> обычное конечное число элементов
// omega_coeff  > 0 -> бесконечное число элементов
class Ordinal {
private:
    unsigned long long omega_coeff; // сколько раз ω
    unsigned long long finite;      // конечный хвост

    //приватный конструктор — наружу только через фабрики
    Ordinal(unsigned long long k, unsigned long long n): omega_coeff(k), finite(n) {}

public:
    //пустой конструктор — это ноль (на случай если Ordinal лежит полем)
    Ordinal(): omega_coeff(0), finite(0) {}

    //фабрики
    static Ordinal Finite(unsigned long long n) {
        return Ordinal(0, n);
    }
    static Ordinal Omega(unsigned long long k = 1) {
        return Ordinal(k, 0);
    }
    static Ordinal FromParts(unsigned long long k, unsigned long long n) {
        return Ordinal(k, n);
    }

    //геттеры — нужны LazySequence, чтобы декодировать индекс в сегмент
    unsigned long long GetOmegaCoeff() const {
        return omega_coeff;
    }
    unsigned long long GetFinite() const {
        return finite;
    }

    //предикаты
    bool IsZero() const {
        return omega_coeff == 0 && finite == 0;
    }
    bool IsFinite() const {
        return omega_coeff == 0;
    }
    bool IsInfinite() const {
        return omega_coeff > 0;
    }

    //сложение — НЕ коммутативно (2 + ω = ω, но ω + 2 = ω+2) :(
    Ordinal operator+(const Ordinal& other) const {
        //если правое слагаемое бесконечно — конечный хвост левого поглощается
        if (other.IsInfinite()) {
            unsigned long long k = omega_coeff + other.omega_coeff;
            if (k < omega_coeff) // тк сумма меньше слагаемого -> переполнение
                throw OutOfRange("переполнение ω-коэффициента");
            return Ordinal(k, other.finite);
        }
        //правое конечно — прибавляем к хвосту левого
        unsigned long long n = finite + other.finite;
        if (n < finite) // тк сумма меньше слагаемого -> переполнение
            throw OutOfRange("переполнение конечной части");
        return Ordinal(omega_coeff, n);
    }

    //сравнения
    bool operator==(const Ordinal& other) const {
        return omega_coeff == other.omega_coeff && finite == other.finite;
    }
    bool operator!=(const Ordinal& other) const {
        return !(*this == other);
    }
    bool operator<(const Ordinal& other) const {
        if (omega_coeff != other.omega_coeff)
            return omega_coeff < other.omega_coeff;
        return finite < other.finite;
    }
    bool operator<=(const Ordinal& other) const {
        return *this < other || *this == other;
    }
    bool operator>(const Ordinal& other) const {
        return other < *this; // тут поменяли местами
    }
    bool operator>=(const Ordinal& other) const {
        return other <= *this;
    }

    //для вывода: "5", "ω", "ω*2+3"
    std::string ToString() const {
        if (IsFinite())
            return std::to_string(finite);
        std::string s = "ω";
        if (omega_coeff > 1)
            s += "*" + std::to_string(omega_coeff);
        if (finite > 0)
            s += "+" + std::to_string(finite);
        return s;
    }
};

#endif //LAB4_ORDINAL_H
