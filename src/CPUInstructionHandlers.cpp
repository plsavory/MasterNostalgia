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

void CPUZ80::addAdc8Bit(unsigned char &dest, unsigned char value, bool withCarry) {
    unsigned char originalValue = dest;
    dest = dest + (value + (withCarry ? (int) getFlag(CPUFlag::carry) : 0));
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::overflow, dest < originalValue);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, (originalValue ^ dest ^ value) & 0x10);
    setFlag(CPUFlag::carry, !(originalValue & 0xFF));
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
}
/**
 * [cpuZ80::add ADD opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to add with what is stored in dest]
 */
void CPUZ80::add8Bit(unsigned char &dest, unsigned char value) {
    addAdc8Bit(dest, value, false);
}

void CPUZ80::adc8Bit(unsigned char &dest, unsigned char value) {
    addAdc8Bit(dest, value, true);
}

void CPUZ80::addAdc16Bit(unsigned short &dest, unsigned short value, bool withCarry) {
    unsigned short originalValue = dest;
    unsigned long result = dest + (value + (withCarry ? (int) getFlag(CPUFlag::carry) : 0));;
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, (result & 0x0FFF + (result & 0x0FFF)) > 0x0FFF);
    setFlag(CPUFlag::carry, result & 0xFFFF0000);
    setFlag(CPUFlag::sign, Utils::testBit(15, dest));
    dest = result & 0xFFFF;
    setFlag(CPUFlag::overflow, dest < originalValue);
}

void CPUZ80::add16Bit(unsigned short &dest, unsigned short value) {
    addAdc16Bit(dest, value, false);
}

void CPUZ80::adc16Bit(unsigned short &dest, unsigned short value) {
    addAdc16Bit(dest, value, true);
}

void CPUZ80::subSbc8Bit(unsigned char &dest, unsigned char value, bool withCarry) {
    unsigned char originalValue = dest;
    dest = dest - (value - (withCarry ? (int) getFlag(CPUFlag::carry) : 0));
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::overflow, dest > originalValue);
    setFlag(CPUFlag::subtract, true);
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
    subSbc8Bit(dest, value, false);
}

void CPUZ80::sbc8Bit(unsigned char &dest, unsigned char value) {
    subSbc8Bit(dest, value, true);
}

void CPUZ80::dec8Bit(unsigned char &dest) {
    dest = getDec8BitValue(dest);
}

unsigned char CPUZ80::getDec8BitValue(unsigned char initialValue) {
    unsigned char newValue = initialValue - 1;
    setFlag(CPUFlag::zero, newValue == 0);
    setFlag(CPUFlag::overflow, newValue > initialValue);
    setFlag(CPUFlag::subtract, true);
    setFlag(CPUFlag::halfCarry, (initialValue ^ newValue ^ 1) & 0x10);
    setFlag(CPUFlag::sign, Utils::testBit(7, newValue));
    return newValue;
}

void CPUZ80::sbc16Bit(unsigned short &dest, unsigned short value) {
    int result = dest - value - getFlag(CPUFlag::carry);
    unsigned short originalValue = dest;
    dest = result & 0xFFFF;

    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::halfCarry, ((originalValue ^ result ^ value) >> 8) & 0x10);
    setFlag(CPUFlag::carry, result & 0x10000);
    setFlag(CPUFlag::sign, Utils::testBit(15, dest));
    setFlag(CPUFlag::overflow, (value ^ originalValue) & (originalValue ^ result) & 0x8000);
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
void CPUZ80::jpCondition(JPCondition condition) {

    unsigned short jumpLocation = build16BitNumber();
    cyclesTaken = 10; // TODO: Ensure that this is the same for all conditional jump instructions

    if (!hasMetJumpCondition(condition)) {
        return;
    }

    programCounter = jumpLocation;
}

void CPUZ80::jrCondition(JPCondition condition, unsigned char offset) {

    if (!hasMetJumpCondition(condition)) {
        cyclesTaken = 7;
        return;
    }

    programCounter = originalProgramCounterValue + static_cast<signed char>(offset);
    cyclesTaken = 12;
}

void CPUZ80::jr(unsigned char offset) {
    programCounter = originalProgramCounterValue + static_cast<signed char>(offset);
    cyclesTaken = 12;
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
void CPUZ80::exclusiveOr(unsigned char &dest, unsigned char value) {
    unsigned char result = gpRegisters[cpuReg::AF].hi ^ value;
    dest = result;
    setFlag(CPUFlag::carry, false);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::zero, result == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    setFlag(CPUFlag::overflow, result % 2 == 0);
}

void CPUZ80::inc16Bit(unsigned short &target) {
    ++target;
}

void CPUZ80::inc8Bit(unsigned char &target) {
    target = getInc8BitValue(target);
}

unsigned char CPUZ80::getInc8BitValue(unsigned char initialValue) {
    unsigned char newValue = initialValue+1;
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, newValue < initialValue);
    setFlag(CPUFlag::halfCarry, !(initialValue & 0x0F));
    setFlag(CPUFlag::sign, Utils::testBit(7, newValue));
    setFlag(CPUFlag::zero, newValue == 0);
    return newValue;
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
    // TODO do flags need to be set when incrementing/decrementing the registers like in the dec/inc instructions?
    memory->write(gpRegisters[cpuReg::DE].whole++, memory->read(gpRegisters[cpuReg::HL].whole++));

    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, false);
    setFlag(CPUFlag::halfCarry, false);

    if ((--gpRegisters[cpuReg::BC].whole) != 0) {
        programCounter -= 2;
        cyclesTaken = 21;
        return;
    }

    cyclesTaken = 16;
}

void CPUZ80::otir() {
    // TODO do flags need to be set when incrementing/decrementing the registers like in the dec/inc instructions?
    writeIOPort(gpRegisters[cpuReg::BC].lo, memory->read(gpRegisters[cpuReg::HL].whole++));

    setFlag(CPUFlag::subtract, true);
    setFlag(CPUFlag::zero, true);

    if ((--gpRegisters[cpuReg::BC].lo) != 0) {
        programCounter -= 2;
        cyclesTaken = 21;
        return;
    }

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

void CPUZ80::store(unsigned short location, unsigned char hi, unsigned char lo) {
    memory->write(location, lo);
    memory->write(location+1, hi);
}

void CPUZ80::djnz(unsigned short from, unsigned char offset) {

    if (--gpRegisters[cpuReg::BC].hi == 0) {
        cyclesTaken = 7;
        return;
    }

    cyclesTaken = 13;
    programCounter = from + static_cast<signed char>(offset);
}

void CPUZ80::dec16Bit(unsigned short &target) {
    --target;
    // TODO handle any flag changes that are required, so far it seems like this instruction never does though.
}

void CPUZ80::shiftLeft(unsigned char &dest, bool copyPreviousCarryFlagValue) {
    bool previousCarryFlagValue = getFlag(CPUFlag::carry);
    bool bit7 = Utils::testBit(7, dest);

    setFlag(CPUFlag::carry, bit7);
    dest <<= 1;
    Utils::setBit(0, copyPreviousCarryFlagValue ? previousCarryFlagValue : bit7, dest);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, false);
}

void CPUZ80::rlc(unsigned char &dest) {
    shiftLeft(dest, false);
}

void CPUZ80::rl(unsigned char &dest) {
    shiftLeft(dest, true);
}

void CPUZ80::shiftRight(unsigned char &dest, bool copyPreviousCarryFlagValue) {
    bool previousCarryFlagValue = getFlag(CPUFlag::carry);
    bool bit0 = Utils::testBit(0, dest);

    setFlag(CPUFlag::carry, bit0);
    dest >>= 1;
    Utils::setBit(7, copyPreviousCarryFlagValue ? previousCarryFlagValue : bit0, dest);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, false);
}

void CPUZ80::rrc(unsigned char &dest) {
    shiftRight(dest, true);
}

void CPUZ80::rr(unsigned char &dest) {
    shiftRight(dest, false);
}

void CPUZ80::exchange8Bit(unsigned char &register1, unsigned char &register2) {
    unsigned char originalRegister1 = register1;
    register1 = register2;
    register2 = originalRegister1;
}

void CPUZ80::exchange16Bit(unsigned short &register1, unsigned short &register2) {
    unsigned short originalRegister1 = register1;
    register1 = register2;
    register2 = originalRegister1;

}

void CPUZ80::popStackExchange(unsigned short &destinationRegister) {
    unsigned short originalRegisterValue = destinationRegister;
    destinationRegister = popStack16();
    pushStack(originalRegisterValue);
}

void CPUZ80::da(unsigned char &dest) {
    bool requiresFirstAddition = (dest & 0x0F) > 0x09 || getFlag(CPUFlag::halfCarry);
    bool requiresSecondAddition = ((dest & 0xF0) >> 4) > 0x09 || getFlag(CPUFlag::carry);

    if (requiresFirstAddition) {
        dest+=0x6;
    }

    if (requiresSecondAddition) {
        dest+=0x60;
    }

    setFlag(CPUFlag::carry, requiresSecondAddition);
    setFlag(CPUFlag::overflow, dest % 2 == 0);
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    setFlag(CPUFlag::halfCarry, false);
}

void CPUZ80::cpl(unsigned char &dest) {
    dest = ~dest;
    setFlag(CPUFlag::halfCarry, true);
    setFlag(CPUFlag::subtract, true);
}