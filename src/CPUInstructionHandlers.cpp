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

void CPUZ80::ldReg8(unsigned char &dest, unsigned char value, bool modifyFlags) {
    ldReg8(dest, value);

    if (!modifyFlags) {
        return;
    }

    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
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

void CPUZ80::ldReg16(unsigned short &dest, unsigned short value) {
    ldReg16(dest, value, false);
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

void CPUZ80::retCondition(JPCondition condition) {
    if (!hasMetJumpCondition(condition)) {
        cyclesTaken = 5;
        return;
    }

    cyclesTaken = 11;
    programCounter = popStack16();
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

void CPUZ80::ini(bool increment) {
    // TODO do flags need to be set when incrementing/decrementing the registers like in the dec/inc instructions?
    memory->write(gpRegisters[cpuReg::HL].whole, readIOPort(gpRegisters[cpuReg::BC].lo));

    gpRegisters[cpuReg::HL].whole += increment ? 1 : -1;

    setFlag(CPUFlag::subtract, true);
    setFlag(CPUFlag::zero, true);

    cyclesTaken = 16;
    --gpRegisters[cpuReg::BC].hi;
}

void CPUZ80::inir(bool increment) {

    ini(increment);

    if (gpRegisters[cpuReg::BC].hi == 0) {
        return;
    }

    programCounter -= 2;
    cyclesTaken = 21;
}

void CPUZ80::cpi(bool increment) {
    compare8Bit(memory->read(gpRegisters[cpuReg::HL].whole));
    --gpRegisters[cpuReg::BC].whole;
    cyclesTaken = 16;

    gpRegisters[cpuReg::HL].whole += (increment ? 1 : -1);
}

void CPUZ80::cpir(bool increment) {
    cpi(increment);

    if (gpRegisters[cpuReg::BC].whole == 0) {
        return;
    }

    if (getFlag(CPUFlag::zero)) {
        return;
    }

    cyclesTaken = 21;
    programCounter -= 2;
}

void CPUZ80::ldi(bool increment) {
    // TODO do flags need to be set when incrementing/decrementing the registers like in the dec/inc instructions?
    memory->write(gpRegisters[cpuReg::DE].whole, memory->read(gpRegisters[cpuReg::HL].whole));

    unsigned short incrementValue = increment ? 1 : -1;
    gpRegisters[cpuReg::DE].whole += incrementValue;
    gpRegisters[cpuReg::HL].whole += incrementValue;

    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, false);
    setFlag(CPUFlag::halfCarry, false);
    --gpRegisters[cpuReg::BC].whole;
    cyclesTaken = 16;
}

void CPUZ80::ldir(bool increment) {

    ldi(increment);

    if (gpRegisters[cpuReg::BC].whole == 0) {
        return;
    }

    programCounter -= 2;
    cyclesTaken = 21;
}

void CPUZ80::outi(bool increment) {
    // TODO do flags need to be set when incrementing/decrementing the registers like in the dec/inc instructions?
    writeIOPort(gpRegisters[cpuReg::BC].lo, memory->read(gpRegisters[cpuReg::HL].whole));

    gpRegisters[cpuReg::HL].whole += (increment ? 1 : -1);

    setFlag(CPUFlag::subtract, true);
    setFlag(CPUFlag::zero, true);
    --gpRegisters[cpuReg::BC].hi;
    cyclesTaken = 16;
}

void CPUZ80::otir(bool increment) {

    outi(increment);

    if (gpRegisters[cpuReg::BC].hi == 0) {
        return;
    }

    programCounter -= 2;
    cyclesTaken = 21;
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

void CPUZ80::callCondition(JPCondition condition) {
    call(build16BitNumber(), hasMetJumpCondition(condition));
}

void CPUZ80::rst(unsigned short location) {
    pushStack(programCounter);
    programCounter = location;
    cyclesTaken = 11;
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

unsigned char CPUZ80::shiftLeft(unsigned char dest, ShiftBitToCopy copyMode) {
    bool previousCarryFlagValue = getFlag(CPUFlag::carry);
    bool bit7 = Utils::testBit(7, dest);

    setFlag(CPUFlag::carry, bit7);
    dest <<= 1;

    bool copyBitValue = false;

    switch (copyMode) {
        case ShiftBitToCopy::copyPreviousValue:
            copyBitValue = bit7;
            break;
        case ShiftBitToCopy::copyCarryFlag:
            copyBitValue = previousCarryFlagValue;
            break;
        case ShiftBitToCopy::copyNothing:
        case ShiftBitToCopy::copyZero:
            break;
        case ShiftBitToCopy::copyOne:
            copyBitValue = true;
            break;
    }

    if (copyMode != ShiftBitToCopy::copyNothing) {
        Utils::setBit(0, copyBitValue, dest);
    }
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, false);
    return dest;
}

unsigned char CPUZ80::rlc(unsigned char dest) {
    return shiftLeft(dest, ShiftBitToCopy::copyPreviousValue);
}

unsigned char CPUZ80::rl(unsigned char dest) {
    return shiftLeft(dest, ShiftBitToCopy::copyCarryFlag);
}

unsigned char CPUZ80::sla(unsigned char dest) {
    return shiftLeft(dest, ShiftBitToCopy::copyZero);
}

unsigned char CPUZ80::sll(unsigned char dest) {
    return shiftLeft(dest, ShiftBitToCopy::copyOne);
}

unsigned char CPUZ80::shiftRight(unsigned char dest, ShiftBitToCopy copyMode) {
    bool previousCarryFlagValue = getFlag(CPUFlag::carry);
    bool bit0 = Utils::testBit(0, dest);

    setFlag(CPUFlag::carry, bit0);
    dest >>= 1;

    bool copyBitValue = false;

    switch (copyMode) {
        case ShiftBitToCopy::copyPreviousValue:
            copyBitValue = bit0;
            break;
        case ShiftBitToCopy::copyCarryFlag:
            copyBitValue = previousCarryFlagValue;
            break;
        case ShiftBitToCopy::copyNothing:
        case ShiftBitToCopy::copyZero:
            break;
        case ShiftBitToCopy::copyOne:
            copyBitValue = true;
            break;
    }

    if (copyMode != ShiftBitToCopy::copyNothing) {
        Utils::setBit(7, copyBitValue, dest);
    }

    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, false);
    return dest;
}

unsigned char CPUZ80::rrc(unsigned char dest) {
    return shiftRight(dest, ShiftBitToCopy::copyPreviousValue);
}

unsigned char CPUZ80::rr(unsigned char dest) {
    return shiftRight(dest, ShiftBitToCopy::copyCarryFlag);
}

unsigned char CPUZ80::sra(unsigned char dest) {
    return shiftRight(dest, ShiftBitToCopy::copyNothing);
}

unsigned char CPUZ80::srl(unsigned char dest) {
    return shiftRight(dest,  ShiftBitToCopy::copyZero);
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

void CPUZ80::exStack(unsigned short &dest) {
    unsigned short originalRegisterValue = dest;
    dest = popStack16();
    pushStack(originalRegisterValue);
    cyclesTaken = 19;
}

void CPUZ80::readPortToRegister(unsigned char &dest, unsigned char portAddress) {
    dest = readIOPort(portAddress);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, dest % 2 == 0);
    cyclesTaken = 12;
}

void CPUZ80::retn() {
    programCounter = popStack16();
    iff1 = iff2;
    cyclesTaken = 14;
}

void CPUZ80::reti() {
    programCounter = popStack16();
    // TODO send a signal to an I/O device when I have figured out where to send it to.
    cyclesTaken = 14;
}

void CPUZ80::rrd(unsigned char &dest) {
    // TODO test/debug this, there is a lot going on here
    unsigned char memoryCurrentValue = memory->read(gpRegisters[cpuReg::HL].hi);
    unsigned char previousRegisterLowerNibble = dest & 0x0F;
    dest = (gpRegisters[cpuReg::AF].hi & 0xF0) + (memoryCurrentValue & 0x0F);
    memory->write(gpRegisters[cpuReg::HL].whole, (unsigned char)((previousRegisterLowerNibble << 4) + (memoryCurrentValue >> 4)));
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, dest % 2 == 0);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    cyclesTaken = 18;
}

void CPUZ80::rld(unsigned char &dest) {
    // TODO test/debug this, there is a lot going on here
    unsigned char memoryCurrentValue = memory->read(gpRegisters[cpuReg::HL].hi);
    unsigned char previousRegisterLowerNibble = dest & 0x0F;
    dest = (gpRegisters[cpuReg::AF].hi & 0xF0) + ((memoryCurrentValue & 0xF0) >> 4);
    memory->write(gpRegisters[cpuReg::HL].whole, (unsigned char)(((memoryCurrentValue & 0x0F) << 4) + previousRegisterLowerNibble));
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::overflow, dest % 2 == 0);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    cyclesTaken = 18;
}

void CPUZ80::bit(unsigned char bitNumber, unsigned char value) {
    setFlag(CPUFlag::subtract, false);
    setFlag(CPUFlag::halfCarry, true);
    setFlag(CPUFlag::zero, !Utils::testBit(bitNumber, value));
}

unsigned char CPUZ80::res(unsigned char bitNumber, unsigned char value) {
    Utils::setBit(bitNumber, false, value);
    return value;
}

unsigned char CPUZ80::set(unsigned char bitNumber, unsigned char value) {
    Utils::setBit(bitNumber, true, value);
    return value;
}