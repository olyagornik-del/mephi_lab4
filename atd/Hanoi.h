#ifndef LAB3_HANOI_H
#define LAB3_HANOI_H

#include "../my_except.h"
#include "Stack.h"
#include "../core/Sequences/MutableArraySequence.h"

enum class Shape { Circle, Square, Triangle };
enum class Color { Red, Green, Blue };

struct Disk {
    int   size;
    Shape shape;
    Color color;
};

struct Move {int from; int to;};

class Hanoi {
private:
    Stack<Disk> sticks[3];
    MutableArraySequence<Move> moves;
    int start;

    void MoveDisk(int from, int to) {
        Disk d = sticks[from].Peek();
        if ( !sticks[to].IsEmpty() && d.size > sticks[to].Peek().size) {
            throw OutOfRange("to", to, 1, d.size-1);
        }
        sticks[from].Pop();
        sticks[to].Push(d);
        Move m = {from, to};
        moves.Append(m);
    }

    void SolveRec(int n, int from, int to, int via) {
        if (n==0) return;
        SolveRec(n-1, from, via, to);
        MoveDisk(from, to);
        SolveRec(n-1, via, to, from);
    }
public:
    Hanoi(Sequence<Disk>* items, int startStick) {
        if (items == nullptr) throw InvalidArgument("items");
        if (startStick < 0 || startStick > 2) throw OutOfRange("startRod", startStick, 0, 2);
        if (items->GetLength() == 0) throw InvalidArgument("items пуст");

        for (int i = 0; i < items->GetLength(); i++) {
            Disk d = items->Get(i);
            if (!sticks[startStick].IsEmpty() && d.size >= sticks[startStick].Peek().size)
                throw InvalidArgument("диски должны идти от большего к меньшему");
            sticks[startStick].Push(d);
        }
        start = startStick;
    }
    void Solve(int target) {
        if (target < 0 || target > 2) throw OutOfRange("target", target, 0, 2);
        if (target == start)          throw InvalidArgument("target совпадает со start");
        int aux = 3 - start - target;
        SolveRec(sticks[start].Size(), start, target, aux);
    }

    // доступ к результату (для тестов/UI)
    int MovesCount() const {
        return moves.GetLength();
    }
    const Stack<Disk>& Stick(int rod) const {
        if (rod < 0 || rod > 2) throw OutOfRange("rod", rod, 0, 2);
        return sticks[rod];
    }

    Move GetMove(int i) const {
        return moves.Get(i);
    }
};


#endif //LAB3_HANOI_H