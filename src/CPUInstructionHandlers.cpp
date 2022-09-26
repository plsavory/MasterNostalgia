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
void CPUZ80::ldReg16(unsigned short &dest, unsigned short value, bool modifyFlags) {
    dest = value;

    if (!modifyFlags) {
        return;
    }

    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, iff1 || iff2);
}

/**
 * [cpuZ80::adc ADC opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to add with what is stored in dest]
 */
void CPUZ80::add8Bit(unsigned char &dest, unsigned char value) {
    unsigned char originalValue = dest;
    dest+=value;
    handleArithmeticFlags(originalValue, dest, false);
}

/**
 * [cpuZ80::sbc SBC opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to subtract with what is stored in dest]
 */
void CPUZ80::sub8Bit(unsigned char &dest, unsigned char value) {
    unsigned char originalValue = dest;
    dest-=value;
    handleArithmeticFlags(originalValue, dest, true);
}

void CPUZ80::and8Bit(unsigned char &dest, unsigned char value) {
    dest = dest & value;
    setFlag(CPUFlag::carry, false);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, true);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::overflow, (dest % 2) == 0);
}

void CPUZ80::or8Bit(unsigned char &dest, unsigned char value) {
    dest = dest | value;
    setFlag(CPUFlag::carry, false);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::overflow, (dest % 2) == 0);
}

void CPUZ80::handleArithmeticFlags(unsigned char originalValue, unsigned char newValue, bool subtraction) {
    setFlag(CPUFlag::zero, newValue == 0);
    setFlag(CPUFlag::overflow, newValue < originalValue);
    setFlag(CPUFlag::subtract, subtraction);
    setFlag(CPUFlag::halfCarry, !(originalValue & 0x0F));
    setFlag(CPUFlag::carry, !(originalValue & 0xFF));
    setFlag(CPUFlag::sign, Utils::testBit(7, newValue));
}

void CPUZ80::setInterruptMode(unsigned char mode) {
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
    }

    // Get the next two bytes in memory and build an address to jump to
    unsigned short address = build16BitNumber();

    // Only jump if the condition is met
    if (conditionMet) {
        programCounter = address;
    }

    cyclesTaken = 10; // TODO: Ensure that this is the same for all conditional jump instructions
}

/**
 * [CPUZ80::jpImm Jumps to the given memory location immediately following the opcode]
 */
void CPUZ80::jpImm() {
    programCounter = build16BitNumber();
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

void CPUZ80::inc16Bit(unsigned short &target) {
    ++target;
}

void CPUZ80::inc8Bit(unsigned char &target) {
    unsigned char originalValue = target;
    ++target;
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, target < originalValue);
    setFlag(CPUFlag::halfCarry, !(originalValue & 0x0F));
    setFlag(CPUFlag::carry, !(originalValue & 0xFF));
    setFlag(CPUFlag::sign, Utils::testBit(7, target));
    setFlag(CPUFlag::zero, target == 0);
}