//
// Created by Peter Savory on 23/12/2022.
//

#ifndef MASTALGIA_Z80INTERRUPTBUS_H
#define MASTALGIA_Z80INTERRUPTBUS_H

#include <vector>

enum class InterruptType : unsigned char {
    None = 0,
    RESET = 1,
    INT = 2,
    NMI = 3
};

class Z80InterruptBus {
public:

    InterruptType getPendingInterrupt();

    void raiseInterrupt(InterruptType type);

private:

    std::vector<InterruptType> interrupts;
};

#endif //MASTALGIA_Z80INTERRUPTBUS_H
