#ifndef LAB4_SCANVALUES_H
#define LAB4_SCANVALUES_H

// Очищает поток после плохого ввода
void flushInput();
bool scanInt(int &out);
bool scanDouble(double &out);
// ввод размера или номера
int inputSize();
// гоняем пока юзер не введёт число от lo до hi
int scanIntInRange(int lo, int hi);

template <class T> bool scanValue(T &out);
template <> bool scanValue<int>(int &out);
template <> bool scanValue<double>(double &out);
template <> bool scanValue<bool>(bool &out);

#endif //LAB4_SCANVALUES_H
