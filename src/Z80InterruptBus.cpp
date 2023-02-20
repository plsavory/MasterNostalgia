//
// Created by Peter Savory on 23/12/2022.
//

#include "Z80InterruptBus.h"

InterruptType Z80InterruptBus::getPendingInterrupt() {

    if (!interrupts.size()) {
        return InterruptType::None;
    }

    InterruptType next = interrupts[0];

    interrupts.erase(interrupts.begin());
    return next;

}

void Z80InterruptBus::raiseInterrupt(InterruptType type) {

    if (type == InterruptType::None) {
        return;
    }

    interrupts.push_back(type);

}