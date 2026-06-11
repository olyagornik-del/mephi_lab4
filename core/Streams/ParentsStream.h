#ifndef LAB4_PARENTSSTREAM_H
#define LAB4_PARENTSSTREAM_H

#include <cstddef> // size_t
#include "../../my_except.h"

// Родитель поток ТОЛЬКО ДЛЯ ЧТЕНИЯ: последовательный доступ, чтение «съедает» элемент.
// Конкретные источники (последовательность, ленивый список, файл, строка) — ReadStreams.h
template <class T>
class ReadOnlyStream {
public:
    ReadOnlyStream() = default;
    //потоки не копируются (держат файловые дескрипторы/позицию) — запрет для всех наследников
    ReadOnlyStream(const ReadOnlyStream<T>&) = delete;
    ReadOnlyStream<T>& operator=(const ReadOnlyStream<T>&) = delete;

    virtual bool IsEndOfStream() const = 0; // можно ли достигнуть конец
    virtual bool IsFinite() const = 0; // конечен ли поток в принципе (бесконечный никогда не кончится)
    virtual T Read() = 0;// прочитать текущий элемент и перейти к следующему
    virtual size_t GetPosition() const = 0; // сколько элементов прочитано
    virtual bool IsCanSeek() const = 0; // можно ли перемещаться без чтения
    virtual size_t Seek(size_t index) = 0; // перейти на позицию (вернуть достигнутую)
    virtual bool IsCanGoBack() const = 0; // можно ли вернуться назад
    virtual void Open() = 0; // подготовка к чтению (открыть файл и т.п.)
    virtual void Close() = 0; // завершение (освободить дескрипторы)
    virtual ~ReadOnlyStream() = default;
};

// Родитель поток ТОЛЬКО ДЛЯ ЗАПИСИ: добавление элемента в конец.
// Конкретные приёмники (последовательность, файл) — WriteStreams.h
template <class T>
class WriteOnlyStream {
public:
    WriteOnlyStream() = default;
    WriteOnlyStream(const WriteOnlyStream<T>&) = delete;
    WriteOnlyStream<T>& operator=(const WriteOnlyStream<T>&) = delete;

    virtual size_t GetPosition() const = 0; // сколько элементов записано
    virtual size_t Write(const T& item) = 0; // записать в конец, вернуть новую позицию
    virtual void Open() = 0;
    virtual void Close() = 0;
    virtual ~WriteOnlyStream() = default;
};

#endif //LAB4_STREAM_H
