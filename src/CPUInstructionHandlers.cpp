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
 * [cpuZ80::add ADD opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to add with what is stored in dest]
 */
void CPUZ80::add8Bit(unsigned char &dest, unsigned char value) {
    unsigned char originalValue = dest;
    dest+=value;
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::overflow, dest < originalValue);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, (originalValue ^ dest ^ value) & 0x10);
    setFlag(CPUFlag::carry, !(originalValue & 0xFF));
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
}

/**
 * [cpuZ80::sub sub opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to subtract with what is stored in dest]
 */
void CPUZ80::sub8Bit(unsigned char &dest, unsigned char value) {
    unsigned char originalValue = dest;
    dest-=value;
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::overflow, dest < originalValue);
    setFlag(CPUFlag::subtract, true);
    setFlag(CPUFlag::halfCarry, (originalValue ^ dest ^ value) & 0x10);
    setFlag(CPUFlag::carry, !(originalValue & 0xFF));
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
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

    if (!hasMetJumpCondition(condition)) {
        return;
    }

    // Get the next two bytes in memory and build an address to jump to
    programCounter = build16BitNumber();

    cyclesTaken = 10; // TODO: Ensure that this is the same for all conditional jump instructions
}

void CPUZ80::jrCondition(JPCondition condition, unsigned short from, unsigned char offset) {

    if (!hasMetJumpCondition(condition)) {
        return;
    }

    programCounter += static_cast<signed char>(offset);
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

void CPUZ80::compare8Bit(unsigned char valueToSubtract) {
    unsigned char originalValue = gpRegisters[cpuReg::AF].hi;
    unsigned char comparisonResult = gpRegisters[cpuReg::AF].hi - valueToSubtract;
    setFlag(CPUFlag::zero, comparisonResult == 0);
    setFlag(CPUFlag::subtract, true);
    setFlag(CPUFlag::carry, originalValue < valueToSubtract);
    setFlag(CPUFlag::halfCarry, (originalValue ^ comparisonResult ^ valueToSubtract) & 0x10);
    setFlag(CPUFlag::carry, originalValue < valueToSubtract);
    setFlag(CPUFlag::sign, Utils::testBit(7, comparisonResult));
    setFlag(CPUFlag::overflow, false);
}

void CPUZ80::ldir() {
    memory->write(gpRegisters[cpuReg::DE].whole++, memory->read(gpRegisters[cpuReg::HL].whole++));

    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, false);
    setFlag(CPUFlag::halfCarry, false);

    if ((--gpRegisters[cpuReg::BC].whole) != 0) {
        repeatLdir = true;
        cyclesTaken = 21;
        return;
    }

    repeatLdir = false;
    cyclesTaken = 16;
}

void CPUZ80::call(unsigned short location, bool conditionMet) {
    if (!conditionMet) {
        cyclesTaken = 10;
        return;
    }

    pushStack(programCounter);
    programCounter = location;
    cyclesTaken = 17;
}

void CPUZ80::call(unsigned short location) {
    call(location, true);
}