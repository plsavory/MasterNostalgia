/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"

/**
 * [cpuZ80::ldReg8 Load 8-bit value into destination]
 * @param dest  [Reference to destination location]
 * @param value [Value to be inserted into destination]
 */
void CPUZ80::ldReg8(unsigned char &dest, unsigned char value) {
    dest = value;
}

/**
 * [cpuZ80::ldReg16 Load 16-bit value into destination register]
 * @param dest  [Reference to destination location]
 * @param value [Value to be inserted into destination]
 */
void CPUZ80::ldReg16(cpuRegister &dest, unsigned short value) {
    dest.whole = value;
}

/**
 * [cpuZ80::adc ADC opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to add with what is stored in dest]
 */
void CPUZ80::adc(unsigned char &dest, unsigned char value) {
// TODO: Implement this
}

/**
 * [cpuZ80::sbc SBC opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to subtract with what is stored in dest]
 */
void CPUZ80::sbc(unsigned char &dest, unsigned char value) {
// TODO: Implement this
}

void CPUZ80::setInterruptMode(int mode) {
    interruptMode = mode;
    cyclesTaken = 8; // All im operations (as far as I know) take 8 cycles
}

/**
 * [CPUZ80::jpCondition Perform a conditional jump]
 * @param condition [The condition which needs to be true]
 * @param location  [Memory location to jump to]
 */
void CPUZ80::jpCondition(JPCondition condition, unsigned char location) {

    bool conditionMet = false;

    switch (condition) {
        case JPCondition::NZ:
            conditionMet = !getFlag(CPUFlag::zero);
            break;
        case JPCondition::Z:
            conditionMet = getFlag(CPUFlag::zero);
            break;
        case JPCondition::NC:
            conditionMet = !getFlag(CPUFlag::carry);
            break;
        case JPCondition::C:
            conditionMet = getFlag(CPUFlag::carry);
            break;
        case JPCondition::PO:
            conditionMet = !getFlag(CPUFlag::overflow);
            break;
        case JPCondition::PE:
            conditionMet = getFlag(CPUFlag::overflow);
            break;
        case JPCondition::P:
            conditionMet = !getFlag(CPUFlag::sign);
            break;
        case JPCondition::M:
            conditionMet = getFlag(CPUFlag::sign);
            break;
        default:
            return;

            // Get the next two bytes in memory and build an address to jump to
            unsigned short address = build16BitAddress();

            // Only jump if the condition is met
            if (conditionMet) {
                programCounter = address;
            }

            cyclesTaken = 10; // TODO: Ensure that this is the same for all conditional jump instructions
    }
}

/**
 * [CPUZ80::jpImm Jumps to the given memory location immediately following the opcode]
 */
void CPUZ80::jpImm() {
    unsigned short address = build16BitAddress();
    programCounter = address;
    cyclesTaken = 10;
}

/**
 * [CPUZ80::xor XOR value with register A]
 * @param value [description]
 */
void CPUZ80::exclusiveOr(unsigned char value) {
    unsigned char result = gpRegisters[cpuReg::AF].hi ^ value;

    // TODO: Perform flag checks on result.
    gpRegisters[cpuReg::AF].hi = result;
}
