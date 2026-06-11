#ifndef LAB4_READSTREAMS_H
#define LAB4_READSTREAMS_H

#include <functional> // std::function — тип десериализатора
#include <string> // std::string — текст, слова, имя файла
#include <sstream> // std::istringstream — читать из строки как из потока (резка по пробелам)
#include <fstream> // std::ifstream — чтение файла
#include "ParentsStream.h"
#include "../Lazy/LazySequence.h"
#include "../Sequences/Sequence.h"
#include "../Sequences/MutableArraySequence.h"

// чтение из обычной последовательности (данные в памяти, можно прыгать и возвращаться)
template <class T>
class SequenceReadStream : public ReadOnlyStream<T> {
private:
    Sequence<T>* seq; // не владеет, это не наш объект -> не храним его тут
    size_t pos;
public:
    explicit SequenceReadStream(Sequence<T>* seq): seq(seq), pos(0) {
        if (seq == nullptr)
            throw InvalidArgument("seq");
    }

    bool IsEndOfStream() const override {
        return pos >= (size_t)seq->GetLength();
    }
    bool IsFinite() const override { return true; } // последовательность в памяти всегда конечна
    T Read() override {
        if (IsEndOfStream())
            throw EndOfStream();
        T value = seq->Get((int)pos);
        pos++;
        return value;
    }
    size_t GetPosition() const override {
        return pos;
    }
    bool IsCanSeek() const override { return true; }
    size_t Seek(size_t index) override {
        size_t len = (size_t)seq->GetLength();
        pos = index < len ? index : len; // дальше конца не пускаем
        return pos;
    }
    bool IsCanGoBack() const override { return true; }
    void Open() override {}
    void Close() override {}
};

// чтение из ленивого списка (может быть бесконечным)
template <class T>
class LazyReadStream : public ReadOnlyStream<T> {
private:
    LazySequence<T>* seq; // не владеет
    size_t pos;
public:
    explicit LazyReadStream(LazySequence<T>* seq): seq(seq), pos(0) {
        if (seq == nullptr)
            throw InvalidArgument("seq");
    }

    bool IsEndOfStream() const override {
        Ordinal len = seq->GetLength();
        if (len.IsInfinite())
            return false; // бесконечный поток не кончается
        return pos >= (size_t)len.GetFinite();
    }
    bool IsFinite() const override { return seq->GetLength().IsFinite(); } // спрашиваем у ленивого списка
    T Read() override {
        if (IsEndOfStream())
            throw EndOfStream();
        T value = seq->Get((int)pos);
        pos++;
        return value;
    }
    size_t GetPosition() const override { return pos; }
    bool IsCanSeek() const override { return true; }
    size_t Seek(size_t index) override {
        Ordinal len = seq->GetLength();
        if (len.IsFinite()) {
            size_t l = (size_t)len.GetFinite();
            pos = index < l ? index : l;
        } else {
            pos = index;
        }
        return pos;
    }
    bool IsCanGoBack() const override { return true; }
    void Open() override {}
    void Close() override {}
};

// чтение из строки: слова, разделённые пробелами/переводами строк, десериализуются в T
template <class T>
class StringReadStream : public ReadOnlyStream<T> {
private:
    MutableArraySequence<T> items; // элементы, которые раздербали из строки
    size_t pos;
public:
    StringReadStream(const std::string& text, std::function<T(const std::string&)> deserialize)
        : items(), pos(0) {
        std::istringstream iss(text);
        std::string word;
        while (iss >> word) // режем строку на слова по пробелам
            items.Append(deserialize(word)); // каждое слово превращаем в T и запоминаем
    } //запоминается всё, но тк строка конечная это проблема только на оч big строках

    bool IsEndOfStream() const override {
        return pos >= (size_t)items.GetLength();
    }
    bool IsFinite() const override { return true; } // строка всегда конечна
    T Read() override {
        if (IsEndOfStream())
            throw EndOfStream();
        T value = items.Get((int)pos);
        pos++;
        return value;
    }
    size_t GetPosition() const override {
        return pos;
    }
    bool IsCanSeek() const override { return true; }
    size_t Seek(size_t index) override {
        size_t len = (size_t)items.GetLength();
        pos = index < len ? index : len;
        return pos;
    }
    bool IsCanGoBack() const override { return true; }
    void Open() override {}
    void Close() override {}
};

// чтение из текстового файла построчно (только вперёд), каждая строка десериализуется в T
template <class T>
class FileReadStream : public ReadOnlyStream<T> {
private:
    std::string filename;
    std::function<T(const std::string&)> deserialize;
    std::ifstream file;
    std::string nextLine; // буфер следующей строки
    bool hasNext;
    size_t pos;

    //прочитать следующую строку в буфер
    void Prime() {
        hasNext = (bool)std::getline(file, nextLine);
    }
public:
    FileReadStream(const std::string& filename, std::function<T(const std::string&)> deserialize)
        : filename(filename), deserialize(deserialize), hasNext(false), pos(0) {}
    bool IsEndOfStream() const override {
        return !hasNext;
    }
    bool IsFinite() const override { return true; } // файл на диске конечен
    T Read() override {
        if (!hasNext)
            throw EndOfStream();
        T value = deserialize(nextLine);
        pos++;
        Prime();
        return value;
    }
    size_t GetPosition() const override {
        return pos;
    }
    bool IsCanSeek() const override { return false; } // файл читаем только вперёд
    size_t Seek(size_t) override {
        throw InvalidArgument("Seek не поддерживается для файлового потока");
    }
    bool IsCanGoBack() const override { return false; }

    void Open() override {
        file.open(filename);
        if (!file.is_open())
            throw InvalidArgument("файл не открылся для чтения");
        Prime();
    }
    void Close() override {
        if (file.is_open())
            file.close();
    }
    ~FileReadStream() override {
        if (file.is_open())
            file.close();
    }
};

#endif //LAB4_READSTREAMS_H
