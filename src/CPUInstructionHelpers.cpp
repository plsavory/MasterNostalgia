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
    readValue = value;
}

void CPUZ80::ldReg8(unsigned char &dest, unsigned char value, bool modifyFlags) {
    ldReg8(dest, value);

    if (!modifyFlags) {
        return;
    }

    setFlag(CPUFlag::subtractNegative, false);
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

    #ifdef DEBUG_VALUES
    readValue = value;
    #endif

    if (!modifyFlags) {
        return;
    }

    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::overflowParity, iff1 || iff2);
}

void CPUZ80::ldReg16(unsigned short &dest, unsigned short value) {
    ldReg16(dest, value, false);
}

void CPUZ80::addAdc8Bit(unsigned char &dest, unsigned char value, bool withCarry) {
    unsigned char originalValue = dest;

    unsigned char originalValueToAdd = value;

    unsigned short result = dest + value + (withCarry && getFlag(CPUFlag::carry) ? 1 : 0);

    unsigned short carryBits = (originalValue ^ originalValueToAdd ^ result);
    dest = (unsigned char)result;
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::overflowParity, (originalValue & 0x80) == (originalValueToAdd & 0x80) && (originalValue & 0x80) != (result & 0x80));
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, carryBits & 0x10);
    setFlag(CPUFlag::carry, carryBits & 0x100);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    handleUndocumentedFlags(dest);
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

    unsigned short originalValueToAdd = value;

    if (withCarry) {
        value += getFlag(CPUFlag::carry) ? 1 : 0;
    }

    unsigned long result = dest + value;
    dest = result & 0xFFFF;
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, (originalValue ^ dest ^ originalValueToAdd) & 0x1000);
    setFlag(CPUFlag::carry, result > 0xFFFF);

    if (withCarry) {
        setFlag(CPUFlag::sign, Utils::testBit(15, dest));
        setFlag(CPUFlag::zero, dest == 0);
        setFlag(CPUFlag::overflowParity, (originalValue & 0x8000) == (originalValueToAdd & 0x8000) && (originalValueToAdd & 0x8000) != (dest & 0x8000));
    }

    handleUndocumentedFlags((unsigned char)(dest >> 8));
}

void CPUZ80::add16Bit(unsigned short &dest, unsigned short value) {
    addAdc16Bit(dest, value, false);
}

void CPUZ80::adc16Bit(unsigned short &dest, unsigned short value) {
    addAdc16Bit(dest, value, true);
}

void CPUZ80::subSbc8Bit(unsigned char &dest, unsigned char value, bool withCarry) {

    unsigned char originalSubtractValue = value;

    unsigned char originalRegisterValue = dest;
    unsigned short result = dest - value - (withCarry && getFlag(CPUFlag::carry) ? 1 : 0);
    unsigned short carryBits = (originalRegisterValue ^ originalSubtractValue ^ result);
    dest = (unsigned char) result;
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::subtractNegative, true);
    setFlag(CPUFlag::carry, carryBits & 0x100);
    setFlag(CPUFlag::halfCarry, carryBits & 0x10);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    setFlag(CPUFlag::overflowParity, ((originalRegisterValue & 0x80) != (originalSubtractValue & 0x80)) && ((dest & 0x80) == (originalSubtractValue & 0x80)));
    handleUndocumentedFlags(dest);
}

void CPUZ80::subSbc16Bit(unsigned short &dest, unsigned short value, bool withCarry) {
    unsigned short originalValue = dest;

    if (withCarry) {
        value += getFlag(CPUFlag::carry) ? 1 : 0;
    }

    dest = dest - value;
    unsigned char highResult = dest >> 8;
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::subtractNegative, true);
    setFlag(CPUFlag::carry, originalValue < value);
    setFlag(CPUFlag::halfCarry, ((originalValue ^ dest ^ value) >> 8) & 0x10);
    setFlag(CPUFlag::sign, Utils::testBit(15, dest));
    setFlag(CPUFlag::overflowParity, ((originalValue & 0x8000) != (value & 0x8000)) && ((dest & 0x8000) == (value & 0x8000)));
    handleUndocumentedFlags(highResult);
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
    setFlag(CPUFlag::yf, Utils::testBit(5, newValue));
    setFlag(CPUFlag::xf, Utils::testBit(3, newValue));
    setFlag(CPUFlag::zero, newValue == 0);
    setFlag(CPUFlag::overflowParity, newValue == 0x7F);
    setFlag(CPUFlag::subtractNegative, true);
    setFlag(CPUFlag::halfCarry, getHalfCarry(initialValue, newValue));
    setFlag(CPUFlag::sign, Utils::testBit(7, newValue));
    return newValue;
}

void CPUZ80::sbc16Bit(unsigned short &dest, unsigned short value) {
    subSbc16Bit(dest, value, true);
}

void CPUZ80::and8Bit(unsigned char &dest, unsigned char value) {
    dest = dest & value;
    setFlag(CPUFlag::carry, false);
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, true);
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    setFlag(CPUFlag::overflowParity, getParity(dest));
    handleUndocumentedFlags(dest);
}

void CPUZ80::or8Bit(unsigned char &dest, unsigned char value) {
    dest = dest | value;
    setFlag(CPUFlag::carry, false);
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::overflowParity, getParity(dest));
    handleUndocumentedFlags(dest);
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

void CPUZ80::jrCondition(JPCondition condition) {

    signed char offset = signedNB();

    if (!hasMetJumpCondition(condition)) {
        cyclesTaken = 7;
        return;
    }

    programCounter += offset;
    cyclesTaken = 12;
    #ifdef DEBUG_VALUES
    readValue = (unsigned char)offset;
    #endif
}

void CPUZ80::retCondition(JPCondition condition) {
    if (!hasMetJumpCondition(condition)) {
        cyclesTaken = 5;
        return;
    }

    cyclesTaken = 11;
    programCounter = popStack16();
}

void CPUZ80::jr() {
    programCounter += signedNB();
    cyclesTaken = 12;
}

/**
 * [CPUZ80::jpImm Jumps to the given memory location immediately following the opcode]
 */
void CPUZ80::jpImm() {
    programCounter = build16BitNumber();
    #ifdef DEBUG_VALUES
    readValue = programCounter;
    #endif
}

/**
 * [CPUZ80::xor XOR value with register A]
 * @param value [description]
 */
void CPUZ80::exclusiveOr(unsigned char &dest, unsigned char value) {
    unsigned char result = gpRegisters[cpuReg::AF].hi ^ value;
    dest = result;
    setFlag(CPUFlag::carry, false);
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::zero, result == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    setFlag(CPUFlag::overflowParity, getParity(result));
    handleUndocumentedFlags(result);
}

void CPUZ80::inc16Bit(unsigned short &target) {
    ++target;
}

void CPUZ80::inc8Bit(unsigned char &target) {
    target = getInc8BitValue(target);
}

unsigned char CPUZ80::getInc8BitValue(unsigned char initialValue) {
    unsigned char newValue = initialValue+1;
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::overflowParity, newValue == 0x80);
    setFlag(CPUFlag::halfCarry, (initialValue & 0xF) == 0xF);
    setFlag(CPUFlag::sign, Utils::testBit(7, newValue));
    setFlag(CPUFlag::zero, newValue == 0);
    handleUndocumentedFlags(newValue);
    return newValue;
}

void CPUZ80::compare8Bit(unsigned char valueToSubtract) {
    unsigned char aValue = gpRegisters[cpuReg::AF].hi;
    subSbc8Bit(aValue, valueToSubtract, false);
    handleUndocumentedFlags(valueToSubtract);
    #ifdef DEBUG_VALUES
    readValue = valueToSubtract;
    #endif
}

void CPUZ80::ini(bool increment) {
    // TODO do flags need to be set when incrementing/decrementing the registers like in the dec/inc instructions?
    memory->write(gpRegisters[cpuReg::HL].whole, z80Io->read(gpRegisters[cpuReg::BC].lo));

    gpRegisters[cpuReg::HL].whole += increment ? 1 : -1;

    setFlag(CPUFlag::subtractNegative, true);
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
    unsigned char value = memory->read(gpRegisters[cpuReg::HL].whole);
    unsigned char originalRegisterValue = gpRegisters[cpuReg::AF].hi;

    unsigned char result = originalRegisterValue - value;
    unsigned char carryBits = (originalRegisterValue ^ value ^ result);
    unsigned char finalResult = result;

    setFlag(CPUFlag::sign, Utils::testBit(7, result));
    setFlag(CPUFlag::zero, result == 0);
    setFlag(CPUFlag::halfCarry, carryBits & 0x10);

    if (getFlag(CPUFlag::halfCarry)) {
        finalResult -= 1;
    }

    setFlag(CPUFlag::xf, Utils::testBit(3, finalResult));
    setFlag(CPUFlag::yf, Utils::testBit(1, finalResult));

    --gpRegisters[cpuReg::BC].whole;
    setFlag(CPUFlag::overflowParity, gpRegisters[cpuReg::BC].whole != 0);
    setFlag(CPUFlag::subtractNegative, true);
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
    unsigned char valueToWrite = memory->read(gpRegisters[cpuReg::HL].whole);
    memory->write(gpRegisters[cpuReg::DE].whole, valueToWrite);

    unsigned short incrementValue = increment ? 1 : -1;
    gpRegisters[cpuReg::DE].whole += incrementValue;
    gpRegisters[cpuReg::HL].whole += incrementValue;

    --gpRegisters[cpuReg::BC].whole;

    unsigned char n = valueToWrite + gpRegisters[cpuReg::AF].hi;
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::yf, Utils::testBit(1, n));
    setFlag(CPUFlag::xf, Utils::testBit(3, n));
    setFlag(CPUFlag::overflowParity, gpRegisters[cpuReg::BC].whole != 0);
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
    unsigned char valueToWrite = memory->read(gpRegisters[cpuReg::HL].whole);
    portOut(gpRegisters[cpuReg::BC].lo,valueToWrite);

    if (increment) {
        gpRegisters[cpuReg::HL].whole += 1;
    } else {
        gpRegisters[cpuReg::HL].whole -= 1;
    }

    --gpRegisters[cpuReg::BC].hi;
    setFlag(CPUFlag::yf, false);
    setFlag(CPUFlag::xf, false);
    setFlag(CPUFlag::sign, Utils::testBit(7, gpRegisters[cpuReg::BC].hi));
    setFlag(CPUFlag::zero, gpRegisters[cpuReg::BC].hi == 0);

    unsigned short k = valueToWrite + gpRegisters[cpuReg::HL].lo;
    setFlag(CPUFlag::subtractNegative, Utils::testBit(7, valueToWrite));
    setFlag(CPUFlag::halfCarry, k > 0xFF);
    setFlag(CPUFlag::carry, k > 0xFF);
    setFlag(CPUFlag::overflowParity, std::bitset<8>((k & 0x7) ^ gpRegisters[cpuReg::BC].hi).count() % 2 == 0);

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

    #ifdef DEBUG_VALUES
    readValue = location;
    #endif

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

    #ifdef DEBUG_VALUES
    readValue = programCounter;
    #endif
}

void CPUZ80::store(unsigned short location, unsigned char hi, unsigned char lo) {
    memory->write(location, lo);
    memory->write(location+1, hi);
}

void CPUZ80::djnz() {

    signed char offset = signedNB();

    if (--gpRegisters[cpuReg::BC].hi == 0) {
        cyclesTaken = 7;
        return;
    }

    cyclesTaken = 13;
    programCounter += offset;
}

void CPUZ80::dec16Bit(unsigned short &target) {
    --target;
    // TODO handle any flag changes that are required, so far it seems like this instruction never does though.
}

unsigned char CPUZ80::shiftLeft(unsigned char dest, ShiftBitToCopy lowBitCopyMode, bool updateAllFlags) {
    bool previousCarryFlagValue = getFlag(CPUFlag::carry);
    bool bit7 = Utils::testBit(7, dest);

    setFlag(CPUFlag::carry, bit7);

    bool copyBitValue = false;

    switch (lowBitCopyMode) {
        case ShiftBitToCopy::copyOutgoingValue:
            copyBitValue = bit7;
            break;
        case ShiftBitToCopy::copyCarryFlag:
            copyBitValue = previousCarryFlagValue;
            break;
        case ShiftBitToCopy::preserve:
            copyBitValue = Utils::testBit(0, dest);
        case ShiftBitToCopy::copyZero:
            break;
        case ShiftBitToCopy::copyOne:
            copyBitValue = true;
            break;
    }

    dest <<= 1;

    Utils::setBit(0, copyBitValue, dest);

    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, false);
    handleUndocumentedFlags(dest);

    if (updateAllFlags) {
        setFlag(CPUFlag::sign, Utils::testBit(7, dest));
        setFlag(CPUFlag::zero, dest == 0);
        setFlag(CPUFlag::overflowParity, getParity(dest));
    }
    return dest;
}

unsigned char CPUZ80::rlc(unsigned char dest) {
    // 8-bit rotation to the left. The bit leaving on the left is copied into the carry, and to bit 0.
    return shiftLeft(dest, ShiftBitToCopy::copyOutgoingValue, true);
}

void CPUZ80::rlca() {
    // Performs RLC A much quicker, and modifies the flags differently.
    gpRegisters[cpuReg::AF].hi = shiftLeft(gpRegisters[cpuReg::AF].hi, ShiftBitToCopy::copyOutgoingValue, false);
}

unsigned char CPUZ80::rl(unsigned char dest) {
    // 9-bit rotation to the left. the register's bits are shifted left. The carry value is put into 0th bit of the register, and the leaving 7th bit is put into the carry.
    return shiftLeft(dest, ShiftBitToCopy::copyCarryFlag, true);
}

void CPUZ80::rla() {
    // Performs an RL A, but is much faster and S, Z, and P/V flags are preserved.
    gpRegisters[cpuReg::AF].hi = shiftLeft(gpRegisters[cpuReg::AF].hi, ShiftBitToCopy::copyCarryFlag, false);
}

unsigned char CPUZ80::sla(unsigned char dest) {
    // Shift all bits left, copy a 0 onto the low bit
    return shiftLeft(dest, ShiftBitToCopy::copyZero, true);
}

unsigned char CPUZ80::sll(unsigned char dest) {
    // An "undocumented" instruction. Functions like sla, except a 1 is inserted into the low bit.
    return shiftLeft(dest, ShiftBitToCopy::copyOne, true);
}

unsigned char CPUZ80::shiftRight(unsigned char dest, ShiftBitToCopy highBitCopyMode, bool updateAllFlags) {
    bool previousCarryFlagValue = getFlag(CPUFlag::carry);
    bool bit0 = Utils::testBit(0, dest);

    setFlag(CPUFlag::carry, bit0);

    bool copyBitValue = false;

    switch (highBitCopyMode) {
        case ShiftBitToCopy::copyOutgoingValue:
            copyBitValue = bit0;
            break;
        case ShiftBitToCopy::copyCarryFlag:
            copyBitValue = previousCarryFlagValue;
            break;
        case ShiftBitToCopy::preserve:
            copyBitValue = Utils::testBit(7, dest);
            break;
        case ShiftBitToCopy::copyZero:
            break;
        case ShiftBitToCopy::copyOne:
            copyBitValue = true;
            break;
    }

    dest >>= 1;

    Utils::setBit(7, copyBitValue, dest);

    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, false);
    handleUndocumentedFlags(dest);

    if (updateAllFlags) {
        setFlag(CPUFlag::sign, Utils::testBit(7, dest));
        setFlag(CPUFlag::zero, dest == 0);
        setFlag(CPUFlag::overflowParity, getParity(dest));
    }

    return dest;
}

unsigned char CPUZ80::rrc(unsigned char dest) {
    // 8-bit rotation to the right. the bit leaving on the right is copied into the carry, and into bit 7.
    return shiftRight(dest, ShiftBitToCopy::copyOutgoingValue, true);
}

void CPUZ80::rrca() {
    // Performs RRC A faster and modifies the flags differently.
    gpRegisters[cpuReg::AF].hi = shiftRight(gpRegisters[cpuReg::AF].hi, ShiftBitToCopy::copyOutgoingValue, false);
}

unsigned char CPUZ80::rr(unsigned char dest) {
    // 9-bit rotation to the right. The carry is copied into bit 7, and the bit leaving on the right is copied into the carry.
    return shiftRight(dest, ShiftBitToCopy::copyCarryFlag, true);
}

void CPUZ80::rra() {
    // Performs RR A faster, and modifies the flags differently.
    gpRegisters[cpuReg::AF].hi = shiftRight(gpRegisters[cpuReg::AF].hi, ShiftBitToCopy::copyCarryFlag, false);
}

unsigned char CPUZ80::sra(unsigned char dest) {
    // Arithmetic shift right 1 bit, bit 0 goes to carry flag, bit 7 remains unchanged.
    return shiftRight(dest, ShiftBitToCopy::preserve, true);
}

unsigned char CPUZ80::srl(unsigned char dest) {
    // Like SRA, except a 0 is put into bit 7. The bits are all shifted right, with bit 0 put into the carry flag.
    return shiftRight(dest,  ShiftBitToCopy::copyZero, true);
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

void CPUZ80::daa(unsigned char &dest) {

    bool subtract = getFlag(CPUFlag::subtractNegative);

    unsigned char diff = 0;

    if (((dest & 0x0F) > 0x09) || getFlag(CPUFlag::halfCarry)) {
        diff += 0x6;
    }

    if ((dest > 0x99) || getFlag(CPUFlag::carry)) {
        diff += 0x60;
        setFlag(CPUFlag::carry, true);
    }

    if (subtract && !getFlag(CPUFlag::halfCarry)) {
        setFlag(CPUFlag::halfCarry, false);
    } else {
        if (subtract && getFlag(CPUFlag::halfCarry)) {
            setFlag(CPUFlag::halfCarry, (dest & 0xF) < 6);
        } else {
            setFlag(CPUFlag::halfCarry, (dest & 0xF) >= 0xA);
        }
    }

    if (subtract) {
        dest -= diff;
    } else {
        dest += diff;
    }

    setFlag(CPUFlag::overflowParity, getParity(dest));
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    handleUndocumentedFlags(dest);
}

void CPUZ80::cpl(unsigned char &dest) {
    dest = ~dest;
    setFlag(CPUFlag::halfCarry, true);
    setFlag(CPUFlag::subtractNegative, true);
    handleUndocumentedFlags(dest);
}

void CPUZ80::exStack(unsigned short &dest) {
    unsigned short originalRegisterValue = dest;
    dest = popStack16();
    pushStack(originalRegisterValue);
    cyclesTaken = 19;
}

void CPUZ80::readPortToRegister(unsigned char &dest, unsigned char portAddress) {

    dest = z80Io->read(portAddress);

    #ifdef DEBUG_VALUES
        ioPortAddress = portAddress;
        readValue = dest;
    #endif

    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::overflowParity, getParity(dest));
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
    /**
     * Moves the lower four bits of A (0-3) into the upper four bits of (HL) (4-7); moves the upper four bits of (HL) (4-7)
     * into the lower four bits of (HL) (0-3); moves the lower four bits of (HL) (0-3) into the lower four bits of A (0-3).
     */
    unsigned char memoryCurrentValue = memory->read(gpRegisters[cpuReg::HL].whole);
    unsigned char previousRegisterLowerNibble = dest & 0x0F;
    dest = (gpRegisters[cpuReg::AF].hi & 0xF0) + (memoryCurrentValue & 0x0F);
    memory->write(gpRegisters[cpuReg::HL].whole, (unsigned char)((previousRegisterLowerNibble << 4) + (memoryCurrentValue >> 4)));
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::overflowParity, getParity(dest));
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    handleUndocumentedFlags(dest);
    cyclesTaken = 18;
}

void CPUZ80::rld(unsigned char &dest) {
    /**
     * Moves the lower four bits of HL (0-3) into the upper four bits of (HL) (4-7); moves the upper four bits of (HL) (4-7)
     * into the lower four bits of A (0-3); moves the lower four bits of A (0-3) into the lower four bits of (HL) (0-3).
     */
    unsigned char memoryCurrentValue = memory->read(gpRegisters[cpuReg::HL].whole);
    unsigned char previousRegisterLowerNibble = dest & 0x0F;
    dest = (gpRegisters[cpuReg::AF].hi & 0xF0) + ((memoryCurrentValue & 0xF0) >> 4);
    memory->write(gpRegisters[cpuReg::HL].whole, (unsigned char)(((memoryCurrentValue & 0x0F) << 4) + previousRegisterLowerNibble));
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::overflowParity, getParity(dest));
    setFlag(CPUFlag::halfCarry, false);
    setFlag(CPUFlag::zero, dest == 0);
    setFlag(CPUFlag::sign, Utils::testBit(7, dest));
    handleUndocumentedFlags(dest);
    cyclesTaken = 18;
}

inline bool CPUZ80::bitLogic(unsigned char bitNumber, unsigned char value) {
    bool bitIsSet = Utils::testBit(bitNumber, value);
    setFlag(CPUFlag::sign, bitNumber == 7 && bitIsSet);
    setFlag(CPUFlag::zero, !bitIsSet);
    setFlag(CPUFlag::halfCarry, true);
    setFlag(CPUFlag::overflowParity, !bitIsSet);
    setFlag(CPUFlag::subtractNegative, false);
}

void CPUZ80::bit(unsigned char bitNumber, unsigned char value) {
    bool bitIsSet = bitLogic(bitNumber, value);
    setFlag(CPUFlag::yf, bitNumber == 5 && bitIsSet);
    setFlag(CPUFlag::xf, bitNumber == 3 && bitIsSet);
}

void CPUZ80::indexedBit(unsigned char bitNumber, unsigned char value) {
    bitLogic(bitNumber, value);
    unsigned char indexedAddressHi = indexedAddressForCurrentOpcode >> 8;
    setFlag(CPUFlag::yf, Utils::testBit(5, indexedAddressHi));
    setFlag(CPUFlag::xf, Utils::testBit(3, indexedAddressHi));
}

unsigned char CPUZ80::res(unsigned char bitNumber, unsigned char value) {
    Utils::setBit(bitNumber, false, value);
    return value;
}

unsigned char CPUZ80::set(unsigned char bitNumber, unsigned char value) {
    Utils::setBit(bitNumber, true, value);
    return value;
}

unsigned char CPUZ80::portIn(unsigned char port) {
    #ifdef DEBUG_VALUES
    ioPortAddress = port;
    #endif
    return z80Io->read(port);
}

void CPUZ80::portOut(unsigned char port, unsigned char value) {
    z80Io->write(port, value);

    #ifdef DEBUG_VALUES
    ioPortAddress = port;
    readValue = value;
    #endif
}

void CPUZ80::writeMemory(unsigned short location, unsigned char value) {
    memory->write(location, value);

    #ifdef DEBUG_VALUES
    readValue = value;
    memoryAddress = location;
    #endif
}

void CPUZ80::writeMemory(unsigned short location, unsigned short value) {
    memory->write(location, value);

    #ifdef DEBUG_VALUES
    readValue = value;
    memoryAddress = location;
    #endif
}

unsigned char CPUZ80::readMemory(unsigned short location) {
    // TODO this wrapper function has been created for debugging purposes to get console output - refactor later.
    unsigned char value = memory->read(location);
    #ifdef DEBUG_VALUES
    readValue = value;
    memoryAddress = location;
    #endif
    return value;
}

unsigned short CPUZ80::readMemory16Bit(unsigned short location) {
    // TODO this wrapper function has been created for debugging purposes to get console output - refactor later.
    unsigned short value = memory->read16Bit(location);
    #ifdef DEBUG_VALUES
        readValue = value;
        memoryAddress = location;
    #endif
    return value;
}

void CPUZ80::ccf() {
    setFlag(CPUFlag::halfCarry, getFlag(CPUFlag::carry));
    setFlag(CPUFlag::carry, !getFlag(CPUFlag::carry));
    setFlag(CPUFlag::subtractNegative, false);

    // Set XF/YF flags accordingly
    gpRegisters[cpuReg::AF].lo |= gpRegisters[cpuReg::AF].hi & 0x28;
}

void CPUZ80::neg() {
    unsigned char value = 0;
    sub8Bit(value, gpRegisters[cpuReg::AF].hi);
    gpRegisters[cpuReg::AF].hi = value;
}