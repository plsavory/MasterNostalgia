#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"
#include "Exceptions.h"

CPUZ80::CPUZ80(Memory *smsMemory, Z80IO *z80Io) {
    // Store a pointer to the memory object
    memory = smsMemory;
    this->z80Io = z80Io;

    pauseInterruptWaiting = false;

    initialiseOpcodeHandlerPointers();

    cyclesTaken = 0;

    // Reset the CPU to its initial state
    reset();
}

CPUZ80::~CPUZ80() {

}

void CPUZ80::reset() {

    pauseInterruptWaiting = false;
    originalProgramCounterValue = programCounter = 0x0;
    stackPointer = 0xDFF0;
    iff1 = iff2 = true;
    enableInterrupts = false;
    interruptMode = 0;

    for (auto & gpRegister : gpRegisters) {
        gpRegister.whole = 0x0;
    }

    registerI = 0x0;
    registerR = 0x0;

    state = CPUState::Running;

    gpRegisters[cpuReg::BC].whole = 0x0;
    gpRegisters[cpuReg::DE].whole = 0xC714;
    gpRegisters[cpuReg::HL].whole = 0x0293;
    gpRegisters[cpuReg::AF].whole = 0xAB40;
    gpRegisters[cpuReg::IX].whole = 0x7A67;
    gpRegisters[cpuReg::IY].whole = 0x7E3C;
    gpRegisters[cpuReg::WZ].whole = 0x0;
}

int CPUZ80::execute() {
    // This function may be redundant - TODO: Call executeOpcode directly from MasterSystem class if this turns out to be the case in the future.
    if (state == CPUState::Running || state == CPUState::Halt) {
        return executeOpcode();
    }

    return 0;
}

int CPUZ80::executeOpcode() {

    cyclesTaken = 0;
    originalProgramCounterValue = programCounter;
    originalStackPointerValue = stackPointer;
    executedInstructionName = "";
    ioPortAddress = 0x0;
    readValue = 0x0;
    memoryAddress = 0x0;

    // Store the original register values that we are going to output to the console for debugging
    for (int i = 0; i < 10; i++) {
        originalRegisterValues[i] = gpRegisters[i];
    }

    if (enableInterrupts) {
        iff1 = iff2 = true;
        enableInterrupts = false;
    }

    if (pauseInterruptWaiting) {
        pauseInterruptWaiting = false;
        pushStack(programCounter);
        state = CPUState::Running;
        programCounter = 0x66;
        gpRegisters[cpuReg::WZ].whole = programCounter;
    }

    if (z80Io->isVDPRequestingInterrupt() && iff1 && interruptMode == 1) {
        state = CPUState::Running;
        pushStack(programCounter);
        programCounter = 0x38;
        gpRegisters[cpuReg::WZ].whole = programCounter;
        iff1 = iff2 = false;
    }

    if (state == CPUState::Halt) {
        return 4; // TODO not sure what to return here in terms of cycles taken, look into it - assume 4 for now
    }

    unsigned char opcode = NBHideFromTrace();

    // The first 7 bits of R should be incremented upon fetching each instruction.
    registerR += 1;
    registerR &= 0x7F;

    displayOpcodePrefix = 0x0;
    displayOpcode = opcode;

    // Execute the instruction through its opcode handler function pointer
    (this->*standardOpcodeHandlers[opcode])();

#ifdef VERBOSE_MODE

    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(opcode, 0x0, 0x0);
    }

    if (state != CPUState::Error)
        logCPUState();
#endif

    return cyclesTaken;
}

/**
 * [CPUZ80::NB Fetch the next byte in memory]
 * @return [description]
 */
unsigned char CPUZ80::NB() {
    return readMemory(programCounter++);
}

unsigned char CPUZ80::NBHideFromTrace() {
    // Fetch the next byte without updating the trace variables that I use for debugging
    return memory->read(programCounter++);
}

/**
 * Returns the signed byte located at the program counter in memory and increments the program counter
 * @return
 */
signed char CPUZ80::signedNB() {
    return (signed char)readMemory(programCounter++);
}

unsigned short CPUZ80::getIndexedOffsetAddress(unsigned short registerValue) {
    return registerValue + signedNB();
}

/**
 * [CPUZ80::extendedOpcodes Handles extended opcodes]
 */
void CPUZ80::extendedOpcodes() {

    unsigned char opcode = NBHideFromTrace();
    displayOpcode = opcode;
    displayOpcodePrefix = 0xED;

    (this->*extendedOpcodeHandlers[opcode])();

#ifdef VERBOSE_MODE
    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(0xED, opcode, 0x0);
    }
#endif
}

void CPUZ80::indexOpcodes(cpuReg indexRegister) {
    unsigned char opcode = NBHideFromTrace();
    displayOpcode = opcode;

    indexRegisterForCurrentOpcode = indexRegister;
    (this->*indexOpcodeHandlers[opcode])();

#ifdef VERBOSE_MODE
    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(indexRegister == cpuReg::IX ? 0xDD : 0xFD, opcode, 0x0);
    }
#endif
}

/**
 * [logCPUState Log the CPU's current state to the console]
 */
void CPUZ80::logCPUState() {
    std::cout << std::uppercase << Utils::padString(executedInstructionName, 15) << "BC="
              << Utils::formatHexNumber(originalRegisterValues[cpuReg::BC].whole) << " DE=" << Utils::formatHexNumber(originalRegisterValues[cpuReg::DE].whole) << " HL="
              << Utils::formatHexNumber(originalRegisterValues[cpuReg::HL].whole) << " AF=" << Utils::formatHexNumber(originalRegisterValues[cpuReg::AF].whole) << " IX="
              << Utils::formatHexNumber(originalRegisterValues[cpuReg::IX].whole) << " IY=" << Utils::formatHexNumber(originalRegisterValues[cpuReg::IY].whole) << " SP="
              << Utils::formatHexNumber(originalStackPointerValue) << " PC=" << Utils::formatHexNumber(originalProgramCounterValue) << std::endl;
}

void CPUZ80::setFlag(CPUFlag flag, bool value) {
    Utils::setBit(flag, value, gpRegisters[cpuReg::AF].lo);
}

bool CPUZ80::getFlag(CPUFlag flag) {
    return Utils::testBit(flag, gpRegisters[cpuReg::AF].lo);
}

/**
 * [CPUZ80::build16BitAddress When called, increments the pc by 4 and builds a 16-bit number]
 * @return [The memory address]
 */
unsigned short CPUZ80::build16BitNumber() {
    unsigned char addrLo = NBHideFromTrace();
    unsigned char addrHi = NBHideFromTrace();

    return (addrLo + (addrHi << 8));
}

/**
 * [CPUZ80::get16BitRelativeValue Returns the value stored within the given memory address]
 * @return [description]
 */
unsigned char CPUZ80::getIndirectValue() {
    return readMemory(build16BitNumber());
}

/**
 * [CPUZ80::get16BitRelativeValue Returns the value stored within the given memory address]
 * @return [description]
 */
unsigned char CPUZ80::getIndirectValue(unsigned short address) {
    return readMemory(address);
}

/**
 * Returns true if the given jump condition is met
 * @param condition
 * @return
 */
bool CPUZ80::hasMetJumpCondition(JPCondition condition) {
    switch (condition) {
        case JPCondition::NZ:
            return !getFlag(CPUFlag::zero);
        case JPCondition::Z:
            return getFlag(CPUFlag::zero);
        case JPCondition::NC:
            return !getFlag(CPUFlag::carry);
        case JPCondition::C:
            return getFlag(CPUFlag::carry);
        case JPCondition::PO:
            return !getFlag(CPUFlag::overflowParity);
        case JPCondition::PE:
            return getFlag(CPUFlag::overflowParity);
        case JPCondition::P:
            return !getFlag(CPUFlag::sign);
        case JPCondition::M:
            return getFlag(CPUFlag::sign);
        default:
            throw Z80Exception("Unhandled jump condition");
    }
}

void CPUZ80::pushStack(unsigned char value) {
    writeMemory(--stackPointer, value);
}

void CPUZ80::pushStack(unsigned short value) {
    unsigned char hi = value >> 8;
    unsigned char lo = value & 0x00FF;
    pushStack(hi);
    pushStack(lo);
}

unsigned char CPUZ80::popStack() {
    return readMemory(stackPointer++);
}

unsigned short CPUZ80::popStack16() {
    return (popStack() + (popStack()<<8));
}

void CPUZ80::bitOpcodes() {

    unsigned char opcode = NBHideFromTrace();
    displayOpcodePrefix = 0xCB;
    displayOpcode = opcode;

    (this->*bitOpcodeHandlers[opcode])();

#ifdef VERBOSE_MODE
    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(0xCB, opcode, 0x0);
    }
#endif

}

void CPUZ80::indexBitOpcodes(cpuReg indexRegister) {
    // Opcode format is offset, opcode
    indexedAddressForCurrentOpcode = gpRegisters[indexRegister].whole + signedNB();
    gpRegisters[cpuReg::WZ].whole = indexedAddressForCurrentOpcode;
    unsigned char opcode = NBHideFromTrace();
    displayOpcode = opcode;

    (this->*indexBitOpcodeHandlers[opcode])();
}

void CPUZ80::raisePauseInterrupt() {
    pauseInterruptWaiting = true;
}

void CPUZ80::initialiseOpcodeHandlerPointers() {
    standardOpcodeHandlers[0x00] = &CPUZ80::standardOpcodeHandler0x00;
    standardOpcodeHandlers[0x01] = &CPUZ80::standardOpcodeHandler0x01;
    standardOpcodeHandlers[0x02] = &CPUZ80::standardOpcodeHandler0x02;
    standardOpcodeHandlers[0x03] = &CPUZ80::standardOpcodeHandler0x03;
    standardOpcodeHandlers[0x04] = &CPUZ80::standardOpcodeHandler0x04;
    standardOpcodeHandlers[0x05] = &CPUZ80::standardOpcodeHandler0x05;
    standardOpcodeHandlers[0x06] = &CPUZ80::standardOpcodeHandler0x06;
    standardOpcodeHandlers[0x07] = &CPUZ80::standardOpcodeHandler0x07;
    standardOpcodeHandlers[0x08] = &CPUZ80::standardOpcodeHandler0x08;
    standardOpcodeHandlers[0x09] = &CPUZ80::standardOpcodeHandler0x09;
    standardOpcodeHandlers[0x0A] = &CPUZ80::standardOpcodeHandler0x0A;
    standardOpcodeHandlers[0x0B] = &CPUZ80::standardOpcodeHandler0x0B;
    standardOpcodeHandlers[0x0C] = &CPUZ80::standardOpcodeHandler0x0C;
    standardOpcodeHandlers[0x0D] = &CPUZ80::standardOpcodeHandler0x0D;
    standardOpcodeHandlers[0x0E] = &CPUZ80::standardOpcodeHandler0x0E;
    standardOpcodeHandlers[0x0F] = &CPUZ80::standardOpcodeHandler0x0F;
    standardOpcodeHandlers[0x10] = &CPUZ80::standardOpcodeHandler0x10;
    standardOpcodeHandlers[0x11] = &CPUZ80::standardOpcodeHandler0x11;
    standardOpcodeHandlers[0x12] = &CPUZ80::standardOpcodeHandler0x12;
    standardOpcodeHandlers[0x13] = &CPUZ80::standardOpcodeHandler0x13;
    standardOpcodeHandlers[0x14] = &CPUZ80::standardOpcodeHandler0x14;
    standardOpcodeHandlers[0x15] = &CPUZ80::standardOpcodeHandler0x15;
    standardOpcodeHandlers[0x16] = &CPUZ80::standardOpcodeHandler0x16;
    standardOpcodeHandlers[0x17] = &CPUZ80::standardOpcodeHandler0x17;
    standardOpcodeHandlers[0x18] = &CPUZ80::standardOpcodeHandler0x18;
    standardOpcodeHandlers[0x19] = &CPUZ80::standardOpcodeHandler0x19;
    standardOpcodeHandlers[0x1A] = &CPUZ80::standardOpcodeHandler0x1A;
    standardOpcodeHandlers[0x1B] = &CPUZ80::standardOpcodeHandler0x1B;
    standardOpcodeHandlers[0x1C] = &CPUZ80::standardOpcodeHandler0x1C;
    standardOpcodeHandlers[0x1D] = &CPUZ80::standardOpcodeHandler0x1D;
    standardOpcodeHandlers[0x1E] = &CPUZ80::standardOpcodeHandler0x1E;
    standardOpcodeHandlers[0x1F] = &CPUZ80::standardOpcodeHandler0x1F;
    standardOpcodeHandlers[0x20] = &CPUZ80::standardOpcodeHandler0x20;
    standardOpcodeHandlers[0x21] = &CPUZ80::standardOpcodeHandler0x21;
    standardOpcodeHandlers[0x22] = &CPUZ80::standardOpcodeHandler0x22;
    standardOpcodeHandlers[0x23] = &CPUZ80::standardOpcodeHandler0x23;
    standardOpcodeHandlers[0x24] = &CPUZ80::standardOpcodeHandler0x24;
    standardOpcodeHandlers[0x25] = &CPUZ80::standardOpcodeHandler0x25;
    standardOpcodeHandlers[0x26] = &CPUZ80::standardOpcodeHandler0x26;
    standardOpcodeHandlers[0x27] = &CPUZ80::standardOpcodeHandler0x27;
    standardOpcodeHandlers[0x28] = &CPUZ80::standardOpcodeHandler0x28;
    standardOpcodeHandlers[0x29] = &CPUZ80::standardOpcodeHandler0x29;
    standardOpcodeHandlers[0x2A] = &CPUZ80::standardOpcodeHandler0x2A;
    standardOpcodeHandlers[0x2B] = &CPUZ80::standardOpcodeHandler0x2B;
    standardOpcodeHandlers[0x2C] = &CPUZ80::standardOpcodeHandler0x2C;
    standardOpcodeHandlers[0x2D] = &CPUZ80::standardOpcodeHandler0x2D;
    standardOpcodeHandlers[0x2E] = &CPUZ80::standardOpcodeHandler0x2E;
    standardOpcodeHandlers[0x2F] = &CPUZ80::standardOpcodeHandler0x2F;
    standardOpcodeHandlers[0x30] = &CPUZ80::standardOpcodeHandler0x30;
    standardOpcodeHandlers[0x31] = &CPUZ80::standardOpcodeHandler0x31;
    standardOpcodeHandlers[0x32] = &CPUZ80::standardOpcodeHandler0x32;
    standardOpcodeHandlers[0x33] = &CPUZ80::standardOpcodeHandler0x33;
    standardOpcodeHandlers[0x34] = &CPUZ80::standardOpcodeHandler0x34;
    standardOpcodeHandlers[0x35] = &CPUZ80::standardOpcodeHandler0x35;
    standardOpcodeHandlers[0x36] = &CPUZ80::standardOpcodeHandler0x36;
    standardOpcodeHandlers[0x37] = &CPUZ80::standardOpcodeHandler0x37;
    standardOpcodeHandlers[0x38] = &CPUZ80::standardOpcodeHandler0x38;
    standardOpcodeHandlers[0x39] = &CPUZ80::standardOpcodeHandler0x39;
    standardOpcodeHandlers[0x3A] = &CPUZ80::standardOpcodeHandler0x3A;
    standardOpcodeHandlers[0x3B] = &CPUZ80::standardOpcodeHandler0x3B;
    standardOpcodeHandlers[0x3C] = &CPUZ80::standardOpcodeHandler0x3C;
    standardOpcodeHandlers[0x3D] = &CPUZ80::standardOpcodeHandler0x3D;
    standardOpcodeHandlers[0x3E] = &CPUZ80::standardOpcodeHandler0x3E;
    standardOpcodeHandlers[0x3F] = &CPUZ80::standardOpcodeHandler0x3F;
    standardOpcodeHandlers[0x40] = &CPUZ80::standardOpcodeHandler0x40;
    standardOpcodeHandlers[0x41] = &CPUZ80::standardOpcodeHandler0x41;
    standardOpcodeHandlers[0x42] = &CPUZ80::standardOpcodeHandler0x42;
    standardOpcodeHandlers[0x43] = &CPUZ80::standardOpcodeHandler0x43;
    standardOpcodeHandlers[0x44] = &CPUZ80::standardOpcodeHandler0x44;
    standardOpcodeHandlers[0x45] = &CPUZ80::standardOpcodeHandler0x45;
    standardOpcodeHandlers[0x46] = &CPUZ80::standardOpcodeHandler0x46;
    standardOpcodeHandlers[0x47] = &CPUZ80::standardOpcodeHandler0x47;
    standardOpcodeHandlers[0x48] = &CPUZ80::standardOpcodeHandler0x48;
    standardOpcodeHandlers[0x49] = &CPUZ80::standardOpcodeHandler0x49;
    standardOpcodeHandlers[0x4A] = &CPUZ80::standardOpcodeHandler0x4A;
    standardOpcodeHandlers[0x4B] = &CPUZ80::standardOpcodeHandler0x4B;
    standardOpcodeHandlers[0x4C] = &CPUZ80::standardOpcodeHandler0x4C;
    standardOpcodeHandlers[0x4D] = &CPUZ80::standardOpcodeHandler0x4D;
    standardOpcodeHandlers[0x4E] = &CPUZ80::standardOpcodeHandler0x4E;
    standardOpcodeHandlers[0x4F] = &CPUZ80::standardOpcodeHandler0x4F;
    standardOpcodeHandlers[0x50] = &CPUZ80::standardOpcodeHandler0x50;
    standardOpcodeHandlers[0x51] = &CPUZ80::standardOpcodeHandler0x51;
    standardOpcodeHandlers[0x52] = &CPUZ80::standardOpcodeHandler0x52;
    standardOpcodeHandlers[0x53] = &CPUZ80::standardOpcodeHandler0x53;
    standardOpcodeHandlers[0x54] = &CPUZ80::standardOpcodeHandler0x54;
    standardOpcodeHandlers[0x55] = &CPUZ80::standardOpcodeHandler0x55;
    standardOpcodeHandlers[0x56] = &CPUZ80::standardOpcodeHandler0x56;
    standardOpcodeHandlers[0x57] = &CPUZ80::standardOpcodeHandler0x57;
    standardOpcodeHandlers[0x58] = &CPUZ80::standardOpcodeHandler0x58;
    standardOpcodeHandlers[0x59] = &CPUZ80::standardOpcodeHandler0x59;
    standardOpcodeHandlers[0x5A] = &CPUZ80::standardOpcodeHandler0x5A;
    standardOpcodeHandlers[0x5B] = &CPUZ80::standardOpcodeHandler0x5B;
    standardOpcodeHandlers[0x5C] = &CPUZ80::standardOpcodeHandler0x5C;
    standardOpcodeHandlers[0x5D] = &CPUZ80::standardOpcodeHandler0x5D;
    standardOpcodeHandlers[0x5E] = &CPUZ80::standardOpcodeHandler0x5E;
    standardOpcodeHandlers[0x5F] = &CPUZ80::standardOpcodeHandler0x5F;
    standardOpcodeHandlers[0x60] = &CPUZ80::standardOpcodeHandler0x60;
    standardOpcodeHandlers[0x61] = &CPUZ80::standardOpcodeHandler0x61;
    standardOpcodeHandlers[0x62] = &CPUZ80::standardOpcodeHandler0x62;
    standardOpcodeHandlers[0x63] = &CPUZ80::standardOpcodeHandler0x63;
    standardOpcodeHandlers[0x64] = &CPUZ80::standardOpcodeHandler0x64;
    standardOpcodeHandlers[0x65] = &CPUZ80::standardOpcodeHandler0x65;
    standardOpcodeHandlers[0x66] = &CPUZ80::standardOpcodeHandler0x66;
    standardOpcodeHandlers[0x67] = &CPUZ80::standardOpcodeHandler0x67;
    standardOpcodeHandlers[0x68] = &CPUZ80::standardOpcodeHandler0x68;
    standardOpcodeHandlers[0x69] = &CPUZ80::standardOpcodeHandler0x69;
    standardOpcodeHandlers[0x6A] = &CPUZ80::standardOpcodeHandler0x6A;
    standardOpcodeHandlers[0x6B] = &CPUZ80::standardOpcodeHandler0x6B;
    standardOpcodeHandlers[0x6C] = &CPUZ80::standardOpcodeHandler0x6C;
    standardOpcodeHandlers[0x6D] = &CPUZ80::standardOpcodeHandler0x6D;
    standardOpcodeHandlers[0x6E] = &CPUZ80::standardOpcodeHandler0x6E;
    standardOpcodeHandlers[0x6F] = &CPUZ80::standardOpcodeHandler0x6F;
    standardOpcodeHandlers[0x70] = &CPUZ80::standardOpcodeHandler0x70;
    standardOpcodeHandlers[0x71] = &CPUZ80::standardOpcodeHandler0x71;
    standardOpcodeHandlers[0x72] = &CPUZ80::standardOpcodeHandler0x72;
    standardOpcodeHandlers[0x73] = &CPUZ80::standardOpcodeHandler0x73;
    standardOpcodeHandlers[0x74] = &CPUZ80::standardOpcodeHandler0x74;
    standardOpcodeHandlers[0x75] = &CPUZ80::standardOpcodeHandler0x75;
    standardOpcodeHandlers[0x76] = &CPUZ80::standardOpcodeHandler0x76;
    standardOpcodeHandlers[0x77] = &CPUZ80::standardOpcodeHandler0x77;
    standardOpcodeHandlers[0x78] = &CPUZ80::standardOpcodeHandler0x78;
    standardOpcodeHandlers[0x79] = &CPUZ80::standardOpcodeHandler0x79;
    standardOpcodeHandlers[0x7A] = &CPUZ80::standardOpcodeHandler0x7A;
    standardOpcodeHandlers[0x7B] = &CPUZ80::standardOpcodeHandler0x7B;
    standardOpcodeHandlers[0x7C] = &CPUZ80::standardOpcodeHandler0x7C;
    standardOpcodeHandlers[0x7D] = &CPUZ80::standardOpcodeHandler0x7D;
    standardOpcodeHandlers[0x7E] = &CPUZ80::standardOpcodeHandler0x7E;
    standardOpcodeHandlers[0x7F] = &CPUZ80::standardOpcodeHandler0x7F;
    standardOpcodeHandlers[0x80] = &CPUZ80::standardOpcodeHandler0x80;
    standardOpcodeHandlers[0x81] = &CPUZ80::standardOpcodeHandler0x81;
    standardOpcodeHandlers[0x82] = &CPUZ80::standardOpcodeHandler0x82;
    standardOpcodeHandlers[0x83] = &CPUZ80::standardOpcodeHandler0x83;
    standardOpcodeHandlers[0x84] = &CPUZ80::standardOpcodeHandler0x84;
    standardOpcodeHandlers[0x85] = &CPUZ80::standardOpcodeHandler0x85;
    standardOpcodeHandlers[0x86] = &CPUZ80::standardOpcodeHandler0x86;
    standardOpcodeHandlers[0x87] = &CPUZ80::standardOpcodeHandler0x87;
    standardOpcodeHandlers[0x88] = &CPUZ80::standardOpcodeHandler0x88;
    standardOpcodeHandlers[0x89] = &CPUZ80::standardOpcodeHandler0x89;
    standardOpcodeHandlers[0x8A] = &CPUZ80::standardOpcodeHandler0x8A;
    standardOpcodeHandlers[0x8B] = &CPUZ80::standardOpcodeHandler0x8B;
    standardOpcodeHandlers[0x8C] = &CPUZ80::standardOpcodeHandler0x8C;
    standardOpcodeHandlers[0x8D] = &CPUZ80::standardOpcodeHandler0x8D;
    standardOpcodeHandlers[0x8E] = &CPUZ80::standardOpcodeHandler0x8E;
    standardOpcodeHandlers[0x8F] = &CPUZ80::standardOpcodeHandler0x8F;
    standardOpcodeHandlers[0x90] = &CPUZ80::standardOpcodeHandler0x90;
    standardOpcodeHandlers[0x91] = &CPUZ80::standardOpcodeHandler0x91;
    standardOpcodeHandlers[0x92] = &CPUZ80::standardOpcodeHandler0x92;
    standardOpcodeHandlers[0x93] = &CPUZ80::standardOpcodeHandler0x93;
    standardOpcodeHandlers[0x94] = &CPUZ80::standardOpcodeHandler0x94;
    standardOpcodeHandlers[0x95] = &CPUZ80::standardOpcodeHandler0x95;
    standardOpcodeHandlers[0x96] = &CPUZ80::standardOpcodeHandler0x96;
    standardOpcodeHandlers[0x97] = &CPUZ80::standardOpcodeHandler0x97;
    standardOpcodeHandlers[0x98] = &CPUZ80::standardOpcodeHandler0x98;
    standardOpcodeHandlers[0x99] = &CPUZ80::standardOpcodeHandler0x99;
    standardOpcodeHandlers[0x9A] = &CPUZ80::standardOpcodeHandler0x9A;
    standardOpcodeHandlers[0x9B] = &CPUZ80::standardOpcodeHandler0x9B;
    standardOpcodeHandlers[0x9C] = &CPUZ80::standardOpcodeHandler0x9C;
    standardOpcodeHandlers[0x9D] = &CPUZ80::standardOpcodeHandler0x9D;
    standardOpcodeHandlers[0x9E] = &CPUZ80::standardOpcodeHandler0x9E;
    standardOpcodeHandlers[0x9F] = &CPUZ80::standardOpcodeHandler0x9F;
    standardOpcodeHandlers[0xA0] = &CPUZ80::standardOpcodeHandler0xA0;
    standardOpcodeHandlers[0xA1] = &CPUZ80::standardOpcodeHandler0xA1;
    standardOpcodeHandlers[0xA2] = &CPUZ80::standardOpcodeHandler0xA2;
    standardOpcodeHandlers[0xA3] = &CPUZ80::standardOpcodeHandler0xA3;
    standardOpcodeHandlers[0xA4] = &CPUZ80::standardOpcodeHandler0xA4;
    standardOpcodeHandlers[0xA5] = &CPUZ80::standardOpcodeHandler0xA5;
    standardOpcodeHandlers[0xA6] = &CPUZ80::standardOpcodeHandler0xA6;
    standardOpcodeHandlers[0xA7] = &CPUZ80::standardOpcodeHandler0xA7;
    standardOpcodeHandlers[0xA8] = &CPUZ80::standardOpcodeHandler0xA8;
    standardOpcodeHandlers[0xA9] = &CPUZ80::standardOpcodeHandler0xA9;
    standardOpcodeHandlers[0xAA] = &CPUZ80::standardOpcodeHandler0xAA;
    standardOpcodeHandlers[0xAB] = &CPUZ80::standardOpcodeHandler0xAB;
    standardOpcodeHandlers[0xAC] = &CPUZ80::standardOpcodeHandler0xAC;
    standardOpcodeHandlers[0xAD] = &CPUZ80::standardOpcodeHandler0xAD;
    standardOpcodeHandlers[0xAE] = &CPUZ80::standardOpcodeHandler0xAE;
    standardOpcodeHandlers[0xAF] = &CPUZ80::standardOpcodeHandler0xAF;
    standardOpcodeHandlers[0xB0] = &CPUZ80::standardOpcodeHandler0xB0;
    standardOpcodeHandlers[0xB1] = &CPUZ80::standardOpcodeHandler0xB1;
    standardOpcodeHandlers[0xB2] = &CPUZ80::standardOpcodeHandler0xB2;
    standardOpcodeHandlers[0xB3] = &CPUZ80::standardOpcodeHandler0xB3;
    standardOpcodeHandlers[0xB4] = &CPUZ80::standardOpcodeHandler0xB4;
    standardOpcodeHandlers[0xB5] = &CPUZ80::standardOpcodeHandler0xB5;
    standardOpcodeHandlers[0xB6] = &CPUZ80::standardOpcodeHandler0xB6;
    standardOpcodeHandlers[0xB7] = &CPUZ80::standardOpcodeHandler0xB7;
    standardOpcodeHandlers[0xB8] = &CPUZ80::standardOpcodeHandler0xB8;
    standardOpcodeHandlers[0xB9] = &CPUZ80::standardOpcodeHandler0xB9;
    standardOpcodeHandlers[0xBA] = &CPUZ80::standardOpcodeHandler0xBA;
    standardOpcodeHandlers[0xBB] = &CPUZ80::standardOpcodeHandler0xBB;
    standardOpcodeHandlers[0xBC] = &CPUZ80::standardOpcodeHandler0xBC;
    standardOpcodeHandlers[0xBD] = &CPUZ80::standardOpcodeHandler0xBD;
    standardOpcodeHandlers[0xBE] = &CPUZ80::standardOpcodeHandler0xBE;
    standardOpcodeHandlers[0xBF] = &CPUZ80::standardOpcodeHandler0xBF;
    standardOpcodeHandlers[0xC0] = &CPUZ80::standardOpcodeHandler0xC0;
    standardOpcodeHandlers[0xC1] = &CPUZ80::standardOpcodeHandler0xC1;
    standardOpcodeHandlers[0xC2] = &CPUZ80::standardOpcodeHandler0xC2;
    standardOpcodeHandlers[0xC3] = &CPUZ80::standardOpcodeHandler0xC3;
    standardOpcodeHandlers[0xC4] = &CPUZ80::standardOpcodeHandler0xC4;
    standardOpcodeHandlers[0xC5] = &CPUZ80::standardOpcodeHandler0xC5;
    standardOpcodeHandlers[0xC6] = &CPUZ80::standardOpcodeHandler0xC6;
    standardOpcodeHandlers[0xC7] = &CPUZ80::standardOpcodeHandler0xC7;
    standardOpcodeHandlers[0xC8] = &CPUZ80::standardOpcodeHandler0xC8;
    standardOpcodeHandlers[0xC9] = &CPUZ80::standardOpcodeHandler0xC9;
    standardOpcodeHandlers[0xCA] = &CPUZ80::standardOpcodeHandler0xCA;
    standardOpcodeHandlers[0xCB] = &CPUZ80::standardOpcodeHandler0xCB;
    standardOpcodeHandlers[0xCC] = &CPUZ80::standardOpcodeHandler0xCC;
    standardOpcodeHandlers[0xCD] = &CPUZ80::standardOpcodeHandler0xCD;
    standardOpcodeHandlers[0xCE] = &CPUZ80::standardOpcodeHandler0xCE;
    standardOpcodeHandlers[0xCF] = &CPUZ80::standardOpcodeHandler0xCF;
    standardOpcodeHandlers[0xD0] = &CPUZ80::standardOpcodeHandler0xD0;
    standardOpcodeHandlers[0xD1] = &CPUZ80::standardOpcodeHandler0xD1;
    standardOpcodeHandlers[0xD2] = &CPUZ80::standardOpcodeHandler0xD2;
    standardOpcodeHandlers[0xD3] = &CPUZ80::standardOpcodeHandler0xD3;
    standardOpcodeHandlers[0xD4] = &CPUZ80::standardOpcodeHandler0xD4;
    standardOpcodeHandlers[0xD5] = &CPUZ80::standardOpcodeHandler0xD5;
    standardOpcodeHandlers[0xD6] = &CPUZ80::standardOpcodeHandler0xD6;
    standardOpcodeHandlers[0xD7] = &CPUZ80::standardOpcodeHandler0xD7;
    standardOpcodeHandlers[0xD8] = &CPUZ80::standardOpcodeHandler0xD8;
    standardOpcodeHandlers[0xD9] = &CPUZ80::standardOpcodeHandler0xD9;
    standardOpcodeHandlers[0xDA] = &CPUZ80::standardOpcodeHandler0xDA;
    standardOpcodeHandlers[0xDB] = &CPUZ80::standardOpcodeHandler0xDB;
    standardOpcodeHandlers[0xDC] = &CPUZ80::standardOpcodeHandler0xDC;
    standardOpcodeHandlers[0xDD] = &CPUZ80::standardOpcodeHandler0xDD;
    standardOpcodeHandlers[0xDE] = &CPUZ80::standardOpcodeHandler0xDE;
    standardOpcodeHandlers[0xDF] = &CPUZ80::standardOpcodeHandler0xDF;
    standardOpcodeHandlers[0xE0] = &CPUZ80::standardOpcodeHandler0xE0;
    standardOpcodeHandlers[0xE1] = &CPUZ80::standardOpcodeHandler0xE1;
    standardOpcodeHandlers[0xE2] = &CPUZ80::standardOpcodeHandler0xE2;
    standardOpcodeHandlers[0xE3] = &CPUZ80::standardOpcodeHandler0xE3;
    standardOpcodeHandlers[0xE4] = &CPUZ80::standardOpcodeHandler0xE4;
    standardOpcodeHandlers[0xE5] = &CPUZ80::standardOpcodeHandler0xE5;
    standardOpcodeHandlers[0xE6] = &CPUZ80::standardOpcodeHandler0xE6;
    standardOpcodeHandlers[0xE7] = &CPUZ80::standardOpcodeHandler0xE7;
    standardOpcodeHandlers[0xE8] = &CPUZ80::standardOpcodeHandler0xE8;
    standardOpcodeHandlers[0xE9] = &CPUZ80::standardOpcodeHandler0xE9;
    standardOpcodeHandlers[0xEA] = &CPUZ80::standardOpcodeHandler0xEA;
    standardOpcodeHandlers[0xEB] = &CPUZ80::standardOpcodeHandler0xEB;
    standardOpcodeHandlers[0xEC] = &CPUZ80::standardOpcodeHandler0xEC;
    standardOpcodeHandlers[0xED] = &CPUZ80::standardOpcodeHandler0xED;
    standardOpcodeHandlers[0xEE] = &CPUZ80::standardOpcodeHandler0xEE;
    standardOpcodeHandlers[0xEF] = &CPUZ80::standardOpcodeHandler0xEF;
    standardOpcodeHandlers[0xF0] = &CPUZ80::standardOpcodeHandler0xF0;
    standardOpcodeHandlers[0xF1] = &CPUZ80::standardOpcodeHandler0xF1;
    standardOpcodeHandlers[0xF2] = &CPUZ80::standardOpcodeHandler0xF2;
    standardOpcodeHandlers[0xF3] = &CPUZ80::standardOpcodeHandler0xF3;
    standardOpcodeHandlers[0xF4] = &CPUZ80::standardOpcodeHandler0xF4;
    standardOpcodeHandlers[0xF5] = &CPUZ80::standardOpcodeHandler0xF5;
    standardOpcodeHandlers[0xF6] = &CPUZ80::standardOpcodeHandler0xF6;
    standardOpcodeHandlers[0xF7] = &CPUZ80::standardOpcodeHandler0xF7;
    standardOpcodeHandlers[0xF8] = &CPUZ80::standardOpcodeHandler0xF8;
    standardOpcodeHandlers[0xF9] = &CPUZ80::standardOpcodeHandler0xF9;
    standardOpcodeHandlers[0xFA] = &CPUZ80::standardOpcodeHandler0xFA;
    standardOpcodeHandlers[0xFB] = &CPUZ80::standardOpcodeHandler0xFB;
    standardOpcodeHandlers[0xFC] = &CPUZ80::standardOpcodeHandler0xFC;
    standardOpcodeHandlers[0xFD] = &CPUZ80::standardOpcodeHandler0xFD;
    standardOpcodeHandlers[0xFE] = &CPUZ80::standardOpcodeHandler0xFE;
    standardOpcodeHandlers[0xFF] = &CPUZ80::standardOpcodeHandler0xFF;

    for (int i = 0; i < 0xFF; i++) {
        // Not every possible number for extended opcodes is valid
        extendedOpcodeHandlers[i] = &CPUZ80::opcodeHandlerInvalid;
    }

    extendedOpcodeHandlers[0x40] = &CPUZ80::extendedOpcodeHandler0x40;
    extendedOpcodeHandlers[0x41] = &CPUZ80::extendedOpcodeHandler0x41;
    extendedOpcodeHandlers[0x42] = &CPUZ80::extendedOpcodeHandler0x42;
    extendedOpcodeHandlers[0x43] = &CPUZ80::extendedOpcodeHandler0x43;
    extendedOpcodeHandlers[0x44] = &CPUZ80::extendedOpcodeHandler0x44;
    extendedOpcodeHandlers[0x45] = &CPUZ80::extendedOpcodeHandler0x45;
    extendedOpcodeHandlers[0x46] = &CPUZ80::extendedOpcodeHandler0x46;
    extendedOpcodeHandlers[0x47] = &CPUZ80::extendedOpcodeHandler0x47;
    extendedOpcodeHandlers[0x48] = &CPUZ80::extendedOpcodeHandler0x48;
    extendedOpcodeHandlers[0x49] = &CPUZ80::extendedOpcodeHandler0x49;
    extendedOpcodeHandlers[0x4A] = &CPUZ80::extendedOpcodeHandler0x4A;
    extendedOpcodeHandlers[0x4B] = &CPUZ80::extendedOpcodeHandler0x4B;
    extendedOpcodeHandlers[0x4D] = &CPUZ80::extendedOpcodeHandler0x4D;
    extendedOpcodeHandlers[0x4F] = &CPUZ80::extendedOpcodeHandler0x4F;
    extendedOpcodeHandlers[0x50] = &CPUZ80::extendedOpcodeHandler0x50;
    extendedOpcodeHandlers[0x51] = &CPUZ80::extendedOpcodeHandler0x51;
    extendedOpcodeHandlers[0x52] = &CPUZ80::extendedOpcodeHandler0x52;
    extendedOpcodeHandlers[0x53] = &CPUZ80::extendedOpcodeHandler0x53;
    extendedOpcodeHandlers[0x56] = &CPUZ80::extendedOpcodeHandler0x56;
    extendedOpcodeHandlers[0x57] = &CPUZ80::extendedOpcodeHandler0x57;
    extendedOpcodeHandlers[0x58] = &CPUZ80::extendedOpcodeHandler0x58;
    extendedOpcodeHandlers[0x59] = &CPUZ80::extendedOpcodeHandler0x59;
    extendedOpcodeHandlers[0x5A] = &CPUZ80::extendedOpcodeHandler0x5A;
    extendedOpcodeHandlers[0x5B] = &CPUZ80::extendedOpcodeHandler0x5B;
    extendedOpcodeHandlers[0x5E] = &CPUZ80::extendedOpcodeHandler0x5E;
    extendedOpcodeHandlers[0x5F] = &CPUZ80::extendedOpcodeHandler0x5F;
    extendedOpcodeHandlers[0x60] = &CPUZ80::extendedOpcodeHandler0x60;
    extendedOpcodeHandlers[0x61] = &CPUZ80::extendedOpcodeHandler0x61;
    extendedOpcodeHandlers[0x62] = &CPUZ80::extendedOpcodeHandler0x62;
    extendedOpcodeHandlers[0x63] = &CPUZ80::extendedOpcodeHandler0x63;
    extendedOpcodeHandlers[0x67] = &CPUZ80::extendedOpcodeHandler0x67;
    extendedOpcodeHandlers[0x68] = &CPUZ80::extendedOpcodeHandler0x68;
    extendedOpcodeHandlers[0x69] = &CPUZ80::extendedOpcodeHandler0x69;
    extendedOpcodeHandlers[0x6A] = &CPUZ80::extendedOpcodeHandler0x6A;
    extendedOpcodeHandlers[0x6B] = &CPUZ80::extendedOpcodeHandler0x6B;
    extendedOpcodeHandlers[0x6F] = &CPUZ80::extendedOpcodeHandler0x6F;
    extendedOpcodeHandlers[0x70] = &CPUZ80::extendedOpcodeHandler0x70;
    extendedOpcodeHandlers[0x71] = &CPUZ80::extendedOpcodeHandler0x71;
    extendedOpcodeHandlers[0x72] = &CPUZ80::extendedOpcodeHandler0x72;
    extendedOpcodeHandlers[0x73] = &CPUZ80::extendedOpcodeHandler0x73;
    extendedOpcodeHandlers[0x78] = &CPUZ80::extendedOpcodeHandler0x78;
    extendedOpcodeHandlers[0x79] = &CPUZ80::extendedOpcodeHandler0x79;
    extendedOpcodeHandlers[0x7A] = &CPUZ80::extendedOpcodeHandler0x7A;
    extendedOpcodeHandlers[0x7B] = &CPUZ80::extendedOpcodeHandler0x7B;
    extendedOpcodeHandlers[0xA0] = &CPUZ80::extendedOpcodeHandler0xA0;
    extendedOpcodeHandlers[0xA1] = &CPUZ80::extendedOpcodeHandler0xA1;
    extendedOpcodeHandlers[0xA2] = &CPUZ80::extendedOpcodeHandler0xA2;
    extendedOpcodeHandlers[0xA3] = &CPUZ80::extendedOpcodeHandler0xA3;
    extendedOpcodeHandlers[0xA8] = &CPUZ80::extendedOpcodeHandler0xA8;
    extendedOpcodeHandlers[0xA9] = &CPUZ80::extendedOpcodeHandler0xA9;
    extendedOpcodeHandlers[0xAA] = &CPUZ80::extendedOpcodeHandler0xAA;
    extendedOpcodeHandlers[0xAB] = &CPUZ80::extendedOpcodeHandler0xAB;
    extendedOpcodeHandlers[0xB0] = &CPUZ80::extendedOpcodeHandler0xB0;
    extendedOpcodeHandlers[0xB1] = &CPUZ80::extendedOpcodeHandler0xB1;
    extendedOpcodeHandlers[0xB2] = &CPUZ80::extendedOpcodeHandler0xB2;
    extendedOpcodeHandlers[0xB3] = &CPUZ80::extendedOpcodeHandler0xB3;
    extendedOpcodeHandlers[0xB8] = &CPUZ80::extendedOpcodeHandler0xB8;
    extendedOpcodeHandlers[0xB9] = &CPUZ80::extendedOpcodeHandler0xB9;
    extendedOpcodeHandlers[0xBA] = &CPUZ80::extendedOpcodeHandler0xBA;
    extendedOpcodeHandlers[0xBB] = &CPUZ80::extendedOpcodeHandler0xBB;

    bitOpcodeHandlers[0x00] = &CPUZ80::bitOpcodeHandler0x00;
    bitOpcodeHandlers[0x01] = &CPUZ80::bitOpcodeHandler0x01;
    bitOpcodeHandlers[0x02] = &CPUZ80::bitOpcodeHandler0x02;
    bitOpcodeHandlers[0x03] = &CPUZ80::bitOpcodeHandler0x03;
    bitOpcodeHandlers[0x04] = &CPUZ80::bitOpcodeHandler0x04;
    bitOpcodeHandlers[0x05] = &CPUZ80::bitOpcodeHandler0x05;
    bitOpcodeHandlers[0x06] = &CPUZ80::bitOpcodeHandler0x06;
    bitOpcodeHandlers[0x07] = &CPUZ80::bitOpcodeHandler0x07;
    bitOpcodeHandlers[0x08] = &CPUZ80::bitOpcodeHandler0x08;
    bitOpcodeHandlers[0x09] = &CPUZ80::bitOpcodeHandler0x09;
    bitOpcodeHandlers[0x0A] = &CPUZ80::bitOpcodeHandler0x0A;
    bitOpcodeHandlers[0x0B] = &CPUZ80::bitOpcodeHandler0x0B;
    bitOpcodeHandlers[0x0C] = &CPUZ80::bitOpcodeHandler0x0C;
    bitOpcodeHandlers[0x0D] = &CPUZ80::bitOpcodeHandler0x0D;
    bitOpcodeHandlers[0x0E] = &CPUZ80::bitOpcodeHandler0x0E;
    bitOpcodeHandlers[0x0F] = &CPUZ80::bitOpcodeHandler0x0F;
    bitOpcodeHandlers[0x10] = &CPUZ80::bitOpcodeHandler0x10;
    bitOpcodeHandlers[0x11] = &CPUZ80::bitOpcodeHandler0x11;
    bitOpcodeHandlers[0x12] = &CPUZ80::bitOpcodeHandler0x12;
    bitOpcodeHandlers[0x13] = &CPUZ80::bitOpcodeHandler0x13;
    bitOpcodeHandlers[0x14] = &CPUZ80::bitOpcodeHandler0x14;
    bitOpcodeHandlers[0x15] = &CPUZ80::bitOpcodeHandler0x15;
    bitOpcodeHandlers[0x16] = &CPUZ80::bitOpcodeHandler0x16;
    bitOpcodeHandlers[0x17] = &CPUZ80::bitOpcodeHandler0x17;
    bitOpcodeHandlers[0x18] = &CPUZ80::bitOpcodeHandler0x18;
    bitOpcodeHandlers[0x19] = &CPUZ80::bitOpcodeHandler0x19;
    bitOpcodeHandlers[0x1A] = &CPUZ80::bitOpcodeHandler0x1A;
    bitOpcodeHandlers[0x1B] = &CPUZ80::bitOpcodeHandler0x1B;
    bitOpcodeHandlers[0x1C] = &CPUZ80::bitOpcodeHandler0x1C;
    bitOpcodeHandlers[0x1D] = &CPUZ80::bitOpcodeHandler0x1D;
    bitOpcodeHandlers[0x1E] = &CPUZ80::bitOpcodeHandler0x1E;
    bitOpcodeHandlers[0x1F] = &CPUZ80::bitOpcodeHandler0x1F;
    bitOpcodeHandlers[0x20] = &CPUZ80::bitOpcodeHandler0x20;
    bitOpcodeHandlers[0x21] = &CPUZ80::bitOpcodeHandler0x21;
    bitOpcodeHandlers[0x22] = &CPUZ80::bitOpcodeHandler0x22;
    bitOpcodeHandlers[0x23] = &CPUZ80::bitOpcodeHandler0x23;
    bitOpcodeHandlers[0x24] = &CPUZ80::bitOpcodeHandler0x24;
    bitOpcodeHandlers[0x25] = &CPUZ80::bitOpcodeHandler0x25;
    bitOpcodeHandlers[0x26] = &CPUZ80::bitOpcodeHandler0x26;
    bitOpcodeHandlers[0x27] = &CPUZ80::bitOpcodeHandler0x27;
    bitOpcodeHandlers[0x28] = &CPUZ80::bitOpcodeHandler0x28;
    bitOpcodeHandlers[0x29] = &CPUZ80::bitOpcodeHandler0x29;
    bitOpcodeHandlers[0x2A] = &CPUZ80::bitOpcodeHandler0x2A;
    bitOpcodeHandlers[0x2B] = &CPUZ80::bitOpcodeHandler0x2B;
    bitOpcodeHandlers[0x2C] = &CPUZ80::bitOpcodeHandler0x2C;
    bitOpcodeHandlers[0x2D] = &CPUZ80::bitOpcodeHandler0x2D;
    bitOpcodeHandlers[0x2E] = &CPUZ80::bitOpcodeHandler0x2E;
    bitOpcodeHandlers[0x2F] = &CPUZ80::bitOpcodeHandler0x2F;
    bitOpcodeHandlers[0x30] = &CPUZ80::bitOpcodeHandler0x30;
    bitOpcodeHandlers[0x31] = &CPUZ80::bitOpcodeHandler0x31;
    bitOpcodeHandlers[0x32] = &CPUZ80::bitOpcodeHandler0x32;
    bitOpcodeHandlers[0x33] = &CPUZ80::bitOpcodeHandler0x33;
    bitOpcodeHandlers[0x34] = &CPUZ80::bitOpcodeHandler0x34;
    bitOpcodeHandlers[0x35] = &CPUZ80::bitOpcodeHandler0x35;
    bitOpcodeHandlers[0x36] = &CPUZ80::bitOpcodeHandler0x36;
    bitOpcodeHandlers[0x37] = &CPUZ80::bitOpcodeHandler0x37;
    bitOpcodeHandlers[0x38] = &CPUZ80::bitOpcodeHandler0x38;
    bitOpcodeHandlers[0x39] = &CPUZ80::bitOpcodeHandler0x39;
    bitOpcodeHandlers[0x3A] = &CPUZ80::bitOpcodeHandler0x3A;
    bitOpcodeHandlers[0x3B] = &CPUZ80::bitOpcodeHandler0x3B;
    bitOpcodeHandlers[0x3C] = &CPUZ80::bitOpcodeHandler0x3C;
    bitOpcodeHandlers[0x3D] = &CPUZ80::bitOpcodeHandler0x3D;
    bitOpcodeHandlers[0x3E] = &CPUZ80::bitOpcodeHandler0x3E;
    bitOpcodeHandlers[0x3F] = &CPUZ80::bitOpcodeHandler0x3F;
    bitOpcodeHandlers[0x40] = &CPUZ80::bitOpcodeHandler0x40;
    bitOpcodeHandlers[0x41] = &CPUZ80::bitOpcodeHandler0x41;
    bitOpcodeHandlers[0x42] = &CPUZ80::bitOpcodeHandler0x42;
    bitOpcodeHandlers[0x43] = &CPUZ80::bitOpcodeHandler0x43;
    bitOpcodeHandlers[0x44] = &CPUZ80::bitOpcodeHandler0x44;
    bitOpcodeHandlers[0x45] = &CPUZ80::bitOpcodeHandler0x45;
    bitOpcodeHandlers[0x46] = &CPUZ80::bitOpcodeHandler0x46;
    bitOpcodeHandlers[0x47] = &CPUZ80::bitOpcodeHandler0x47;
    bitOpcodeHandlers[0x48] = &CPUZ80::bitOpcodeHandler0x48;
    bitOpcodeHandlers[0x49] = &CPUZ80::bitOpcodeHandler0x49;
    bitOpcodeHandlers[0x4A] = &CPUZ80::bitOpcodeHandler0x4A;
    bitOpcodeHandlers[0x4B] = &CPUZ80::bitOpcodeHandler0x4B;
    bitOpcodeHandlers[0x4C] = &CPUZ80::bitOpcodeHandler0x4C;
    bitOpcodeHandlers[0x4D] = &CPUZ80::bitOpcodeHandler0x4D;
    bitOpcodeHandlers[0x4E] = &CPUZ80::bitOpcodeHandler0x4E;
    bitOpcodeHandlers[0x4F] = &CPUZ80::bitOpcodeHandler0x4F;
    bitOpcodeHandlers[0x50] = &CPUZ80::bitOpcodeHandler0x50;
    bitOpcodeHandlers[0x51] = &CPUZ80::bitOpcodeHandler0x51;
    bitOpcodeHandlers[0x52] = &CPUZ80::bitOpcodeHandler0x52;
    bitOpcodeHandlers[0x53] = &CPUZ80::bitOpcodeHandler0x53;
    bitOpcodeHandlers[0x54] = &CPUZ80::bitOpcodeHandler0x54;
    bitOpcodeHandlers[0x55] = &CPUZ80::bitOpcodeHandler0x55;
    bitOpcodeHandlers[0x56] = &CPUZ80::bitOpcodeHandler0x56;
    bitOpcodeHandlers[0x57] = &CPUZ80::bitOpcodeHandler0x57;
    bitOpcodeHandlers[0x58] = &CPUZ80::bitOpcodeHandler0x58;
    bitOpcodeHandlers[0x59] = &CPUZ80::bitOpcodeHandler0x59;
    bitOpcodeHandlers[0x5A] = &CPUZ80::bitOpcodeHandler0x5A;
    bitOpcodeHandlers[0x5B] = &CPUZ80::bitOpcodeHandler0x5B;
    bitOpcodeHandlers[0x5C] = &CPUZ80::bitOpcodeHandler0x5C;
    bitOpcodeHandlers[0x5D] = &CPUZ80::bitOpcodeHandler0x5D;
    bitOpcodeHandlers[0x5E] = &CPUZ80::bitOpcodeHandler0x5E;
    bitOpcodeHandlers[0x5F] = &CPUZ80::bitOpcodeHandler0x5F;
    bitOpcodeHandlers[0x60] = &CPUZ80::bitOpcodeHandler0x60;
    bitOpcodeHandlers[0x61] = &CPUZ80::bitOpcodeHandler0x61;
    bitOpcodeHandlers[0x62] = &CPUZ80::bitOpcodeHandler0x62;
    bitOpcodeHandlers[0x63] = &CPUZ80::bitOpcodeHandler0x63;
    bitOpcodeHandlers[0x64] = &CPUZ80::bitOpcodeHandler0x64;
    bitOpcodeHandlers[0x65] = &CPUZ80::bitOpcodeHandler0x65;
    bitOpcodeHandlers[0x66] = &CPUZ80::bitOpcodeHandler0x66;
    bitOpcodeHandlers[0x67] = &CPUZ80::bitOpcodeHandler0x67;
    bitOpcodeHandlers[0x68] = &CPUZ80::bitOpcodeHandler0x68;
    bitOpcodeHandlers[0x69] = &CPUZ80::bitOpcodeHandler0x69;
    bitOpcodeHandlers[0x6A] = &CPUZ80::bitOpcodeHandler0x6A;
    bitOpcodeHandlers[0x6B] = &CPUZ80::bitOpcodeHandler0x6B;
    bitOpcodeHandlers[0x6C] = &CPUZ80::bitOpcodeHandler0x6C;
    bitOpcodeHandlers[0x6D] = &CPUZ80::bitOpcodeHandler0x6D;
    bitOpcodeHandlers[0x6E] = &CPUZ80::bitOpcodeHandler0x6E;
    bitOpcodeHandlers[0x6F] = &CPUZ80::bitOpcodeHandler0x6F;
    bitOpcodeHandlers[0x70] = &CPUZ80::bitOpcodeHandler0x70;
    bitOpcodeHandlers[0x71] = &CPUZ80::bitOpcodeHandler0x71;
    bitOpcodeHandlers[0x72] = &CPUZ80::bitOpcodeHandler0x72;
    bitOpcodeHandlers[0x73] = &CPUZ80::bitOpcodeHandler0x73;
    bitOpcodeHandlers[0x74] = &CPUZ80::bitOpcodeHandler0x74;
    bitOpcodeHandlers[0x75] = &CPUZ80::bitOpcodeHandler0x75;
    bitOpcodeHandlers[0x76] = &CPUZ80::bitOpcodeHandler0x76;
    bitOpcodeHandlers[0x77] = &CPUZ80::bitOpcodeHandler0x77;
    bitOpcodeHandlers[0x78] = &CPUZ80::bitOpcodeHandler0x78;
    bitOpcodeHandlers[0x79] = &CPUZ80::bitOpcodeHandler0x79;
    bitOpcodeHandlers[0x7A] = &CPUZ80::bitOpcodeHandler0x7A;
    bitOpcodeHandlers[0x7B] = &CPUZ80::bitOpcodeHandler0x7B;
    bitOpcodeHandlers[0x7C] = &CPUZ80::bitOpcodeHandler0x7C;
    bitOpcodeHandlers[0x7D] = &CPUZ80::bitOpcodeHandler0x7D;
    bitOpcodeHandlers[0x7E] = &CPUZ80::bitOpcodeHandler0x7E;
    bitOpcodeHandlers[0x7F] = &CPUZ80::bitOpcodeHandler0x7F;
    bitOpcodeHandlers[0x80] = &CPUZ80::bitOpcodeHandler0x80;
    bitOpcodeHandlers[0x81] = &CPUZ80::bitOpcodeHandler0x81;
    bitOpcodeHandlers[0x82] = &CPUZ80::bitOpcodeHandler0x82;
    bitOpcodeHandlers[0x83] = &CPUZ80::bitOpcodeHandler0x83;
    bitOpcodeHandlers[0x84] = &CPUZ80::bitOpcodeHandler0x84;
    bitOpcodeHandlers[0x85] = &CPUZ80::bitOpcodeHandler0x85;
    bitOpcodeHandlers[0x86] = &CPUZ80::bitOpcodeHandler0x86;
    bitOpcodeHandlers[0x87] = &CPUZ80::bitOpcodeHandler0x87;
    bitOpcodeHandlers[0x88] = &CPUZ80::bitOpcodeHandler0x88;
    bitOpcodeHandlers[0x89] = &CPUZ80::bitOpcodeHandler0x89;
    bitOpcodeHandlers[0x8A] = &CPUZ80::bitOpcodeHandler0x8A;
    bitOpcodeHandlers[0x8B] = &CPUZ80::bitOpcodeHandler0x8B;
    bitOpcodeHandlers[0x8C] = &CPUZ80::bitOpcodeHandler0x8C;
    bitOpcodeHandlers[0x8D] = &CPUZ80::bitOpcodeHandler0x8D;
    bitOpcodeHandlers[0x8E] = &CPUZ80::bitOpcodeHandler0x8E;
    bitOpcodeHandlers[0x8F] = &CPUZ80::bitOpcodeHandler0x8F;
    bitOpcodeHandlers[0x90] = &CPUZ80::bitOpcodeHandler0x90;
    bitOpcodeHandlers[0x91] = &CPUZ80::bitOpcodeHandler0x91;
    bitOpcodeHandlers[0x92] = &CPUZ80::bitOpcodeHandler0x92;
    bitOpcodeHandlers[0x93] = &CPUZ80::bitOpcodeHandler0x93;
    bitOpcodeHandlers[0x94] = &CPUZ80::bitOpcodeHandler0x94;
    bitOpcodeHandlers[0x95] = &CPUZ80::bitOpcodeHandler0x95;
    bitOpcodeHandlers[0x96] = &CPUZ80::bitOpcodeHandler0x96;
    bitOpcodeHandlers[0x97] = &CPUZ80::bitOpcodeHandler0x97;
    bitOpcodeHandlers[0x98] = &CPUZ80::bitOpcodeHandler0x98;
    bitOpcodeHandlers[0x99] = &CPUZ80::bitOpcodeHandler0x99;
    bitOpcodeHandlers[0x9A] = &CPUZ80::bitOpcodeHandler0x9A;
    bitOpcodeHandlers[0x9B] = &CPUZ80::bitOpcodeHandler0x9B;
    bitOpcodeHandlers[0x9C] = &CPUZ80::bitOpcodeHandler0x9C;
    bitOpcodeHandlers[0x9D] = &CPUZ80::bitOpcodeHandler0x9D;
    bitOpcodeHandlers[0x9E] = &CPUZ80::bitOpcodeHandler0x9E;
    bitOpcodeHandlers[0x9F] = &CPUZ80::bitOpcodeHandler0x9F;
    bitOpcodeHandlers[0xA0] = &CPUZ80::bitOpcodeHandler0xA0;
    bitOpcodeHandlers[0xA1] = &CPUZ80::bitOpcodeHandler0xA1;
    bitOpcodeHandlers[0xA2] = &CPUZ80::bitOpcodeHandler0xA2;
    bitOpcodeHandlers[0xA3] = &CPUZ80::bitOpcodeHandler0xA3;
    bitOpcodeHandlers[0xA4] = &CPUZ80::bitOpcodeHandler0xA4;
    bitOpcodeHandlers[0xA5] = &CPUZ80::bitOpcodeHandler0xA5;
    bitOpcodeHandlers[0xA6] = &CPUZ80::bitOpcodeHandler0xA6;
    bitOpcodeHandlers[0xA7] = &CPUZ80::bitOpcodeHandler0xA7;
    bitOpcodeHandlers[0xA8] = &CPUZ80::bitOpcodeHandler0xA8;
    bitOpcodeHandlers[0xA9] = &CPUZ80::bitOpcodeHandler0xA9;
    bitOpcodeHandlers[0xAA] = &CPUZ80::bitOpcodeHandler0xAA;
    bitOpcodeHandlers[0xAB] = &CPUZ80::bitOpcodeHandler0xAB;
    bitOpcodeHandlers[0xAC] = &CPUZ80::bitOpcodeHandler0xAC;
    bitOpcodeHandlers[0xAD] = &CPUZ80::bitOpcodeHandler0xAD;
    bitOpcodeHandlers[0xAE] = &CPUZ80::bitOpcodeHandler0xAE;
    bitOpcodeHandlers[0xAF] = &CPUZ80::bitOpcodeHandler0xAF;
    bitOpcodeHandlers[0xB0] = &CPUZ80::bitOpcodeHandler0xB0;
    bitOpcodeHandlers[0xB1] = &CPUZ80::bitOpcodeHandler0xB1;
    bitOpcodeHandlers[0xB2] = &CPUZ80::bitOpcodeHandler0xB2;
    bitOpcodeHandlers[0xB3] = &CPUZ80::bitOpcodeHandler0xB3;
    bitOpcodeHandlers[0xB4] = &CPUZ80::bitOpcodeHandler0xB4;
    bitOpcodeHandlers[0xB5] = &CPUZ80::bitOpcodeHandler0xB5;
    bitOpcodeHandlers[0xB6] = &CPUZ80::bitOpcodeHandler0xB6;
    bitOpcodeHandlers[0xB7] = &CPUZ80::bitOpcodeHandler0xB7;
    bitOpcodeHandlers[0xB8] = &CPUZ80::bitOpcodeHandler0xB8;
    bitOpcodeHandlers[0xB9] = &CPUZ80::bitOpcodeHandler0xB9;
    bitOpcodeHandlers[0xBA] = &CPUZ80::bitOpcodeHandler0xBA;
    bitOpcodeHandlers[0xBB] = &CPUZ80::bitOpcodeHandler0xBB;
    bitOpcodeHandlers[0xBC] = &CPUZ80::bitOpcodeHandler0xBC;
    bitOpcodeHandlers[0xBD] = &CPUZ80::bitOpcodeHandler0xBD;
    bitOpcodeHandlers[0xBE] = &CPUZ80::bitOpcodeHandler0xBE;
    bitOpcodeHandlers[0xBF] = &CPUZ80::bitOpcodeHandler0xBF;
    bitOpcodeHandlers[0xC0] = &CPUZ80::bitOpcodeHandler0xC0;
    bitOpcodeHandlers[0xC1] = &CPUZ80::bitOpcodeHandler0xC1;
    bitOpcodeHandlers[0xC2] = &CPUZ80::bitOpcodeHandler0xC2;
    bitOpcodeHandlers[0xC3] = &CPUZ80::bitOpcodeHandler0xC3;
    bitOpcodeHandlers[0xC4] = &CPUZ80::bitOpcodeHandler0xC4;
    bitOpcodeHandlers[0xC5] = &CPUZ80::bitOpcodeHandler0xC5;
    bitOpcodeHandlers[0xC6] = &CPUZ80::bitOpcodeHandler0xC6;
    bitOpcodeHandlers[0xC7] = &CPUZ80::bitOpcodeHandler0xC7;
    bitOpcodeHandlers[0xC8] = &CPUZ80::bitOpcodeHandler0xC8;
    bitOpcodeHandlers[0xC9] = &CPUZ80::bitOpcodeHandler0xC9;
    bitOpcodeHandlers[0xCA] = &CPUZ80::bitOpcodeHandler0xCA;
    bitOpcodeHandlers[0xCB] = &CPUZ80::bitOpcodeHandler0xCB;
    bitOpcodeHandlers[0xCC] = &CPUZ80::bitOpcodeHandler0xCC;
    bitOpcodeHandlers[0xCD] = &CPUZ80::bitOpcodeHandler0xCD;
    bitOpcodeHandlers[0xCE] = &CPUZ80::bitOpcodeHandler0xCE;
    bitOpcodeHandlers[0xCF] = &CPUZ80::bitOpcodeHandler0xCF;
    bitOpcodeHandlers[0xD0] = &CPUZ80::bitOpcodeHandler0xD0;
    bitOpcodeHandlers[0xD1] = &CPUZ80::bitOpcodeHandler0xD1;
    bitOpcodeHandlers[0xD2] = &CPUZ80::bitOpcodeHandler0xD2;
    bitOpcodeHandlers[0xD3] = &CPUZ80::bitOpcodeHandler0xD3;
    bitOpcodeHandlers[0xD4] = &CPUZ80::bitOpcodeHandler0xD4;
    bitOpcodeHandlers[0xD5] = &CPUZ80::bitOpcodeHandler0xD5;
    bitOpcodeHandlers[0xD6] = &CPUZ80::bitOpcodeHandler0xD6;
    bitOpcodeHandlers[0xD7] = &CPUZ80::bitOpcodeHandler0xD7;
    bitOpcodeHandlers[0xD8] = &CPUZ80::bitOpcodeHandler0xD8;
    bitOpcodeHandlers[0xD9] = &CPUZ80::bitOpcodeHandler0xD9;
    bitOpcodeHandlers[0xDA] = &CPUZ80::bitOpcodeHandler0xDA;
    bitOpcodeHandlers[0xDB] = &CPUZ80::bitOpcodeHandler0xDB;
    bitOpcodeHandlers[0xDC] = &CPUZ80::bitOpcodeHandler0xDC;
    bitOpcodeHandlers[0xDD] = &CPUZ80::bitOpcodeHandler0xDD;
    bitOpcodeHandlers[0xDE] = &CPUZ80::bitOpcodeHandler0xDE;
    bitOpcodeHandlers[0xDF] = &CPUZ80::bitOpcodeHandler0xDF;
    bitOpcodeHandlers[0xE0] = &CPUZ80::bitOpcodeHandler0xE0;
    bitOpcodeHandlers[0xE1] = &CPUZ80::bitOpcodeHandler0xE1;
    bitOpcodeHandlers[0xE2] = &CPUZ80::bitOpcodeHandler0xE2;
    bitOpcodeHandlers[0xE3] = &CPUZ80::bitOpcodeHandler0xE3;
    bitOpcodeHandlers[0xE4] = &CPUZ80::bitOpcodeHandler0xE4;
    bitOpcodeHandlers[0xE5] = &CPUZ80::bitOpcodeHandler0xE5;
    bitOpcodeHandlers[0xE6] = &CPUZ80::bitOpcodeHandler0xE6;
    bitOpcodeHandlers[0xE7] = &CPUZ80::bitOpcodeHandler0xE7;
    bitOpcodeHandlers[0xE8] = &CPUZ80::bitOpcodeHandler0xE8;
    bitOpcodeHandlers[0xE9] = &CPUZ80::bitOpcodeHandler0xE9;
    bitOpcodeHandlers[0xEA] = &CPUZ80::bitOpcodeHandler0xEA;
    bitOpcodeHandlers[0xEB] = &CPUZ80::bitOpcodeHandler0xEB;
    bitOpcodeHandlers[0xEC] = &CPUZ80::bitOpcodeHandler0xEC;
    bitOpcodeHandlers[0xED] = &CPUZ80::bitOpcodeHandler0xED;
    bitOpcodeHandlers[0xEE] = &CPUZ80::bitOpcodeHandler0xEE;
    bitOpcodeHandlers[0xEF] = &CPUZ80::bitOpcodeHandler0xEF;
    bitOpcodeHandlers[0xF0] = &CPUZ80::bitOpcodeHandler0xF0;
    bitOpcodeHandlers[0xF1] = &CPUZ80::bitOpcodeHandler0xF1;
    bitOpcodeHandlers[0xF2] = &CPUZ80::bitOpcodeHandler0xF2;
    bitOpcodeHandlers[0xF3] = &CPUZ80::bitOpcodeHandler0xF3;
    bitOpcodeHandlers[0xF4] = &CPUZ80::bitOpcodeHandler0xF4;
    bitOpcodeHandlers[0xF5] = &CPUZ80::bitOpcodeHandler0xF5;
    bitOpcodeHandlers[0xF6] = &CPUZ80::bitOpcodeHandler0xF6;
    bitOpcodeHandlers[0xF7] = &CPUZ80::bitOpcodeHandler0xF7;
    bitOpcodeHandlers[0xF8] = &CPUZ80::bitOpcodeHandler0xF8;
    bitOpcodeHandlers[0xF9] = &CPUZ80::bitOpcodeHandler0xF9;
    bitOpcodeHandlers[0xFA] = &CPUZ80::bitOpcodeHandler0xFA;
    bitOpcodeHandlers[0xFB] = &CPUZ80::bitOpcodeHandler0xFB;
    bitOpcodeHandlers[0xFC] = &CPUZ80::bitOpcodeHandler0xFC;
    bitOpcodeHandlers[0xFD] = &CPUZ80::bitOpcodeHandler0xFD;
    bitOpcodeHandlers[0xFE] = &CPUZ80::bitOpcodeHandler0xFE;
    bitOpcodeHandlers[0xFF] = &CPUZ80::bitOpcodeHandler0xFF;

    for (int i = 0; i < 0xFF; i++) {
        // Not every possible number for extended opcodes is valid
        indexOpcodeHandlers[i] = &CPUZ80::invalidIndexOpcodeHandler;
    }

    indexOpcodeHandlers[0x04] = &CPUZ80::indexOpcodeHandler0x04;
    indexOpcodeHandlers[0x05] = &CPUZ80::indexOpcodeHandler0x05;
    indexOpcodeHandlers[0x06] = &CPUZ80::indexOpcodeHandler0x06;
    indexOpcodeHandlers[0x09] = &CPUZ80::indexOpcodeHandler0x09;
    indexOpcodeHandlers[0x0C] = &CPUZ80::indexOpcodeHandler0x0C;
    indexOpcodeHandlers[0x0D] = &CPUZ80::indexOpcodeHandler0x0D;
    indexOpcodeHandlers[0x0E] = &CPUZ80::indexOpcodeHandler0x0E;
    indexOpcodeHandlers[0x14] = &CPUZ80::indexOpcodeHandler0x14;
    indexOpcodeHandlers[0x15] = &CPUZ80::indexOpcodeHandler0x15;
    indexOpcodeHandlers[0x16] = &CPUZ80::indexOpcodeHandler0x16;
    indexOpcodeHandlers[0x19] = &CPUZ80::indexOpcodeHandler0x19;
    indexOpcodeHandlers[0x1C] = &CPUZ80::indexOpcodeHandler0x1C;
    indexOpcodeHandlers[0x1D] = &CPUZ80::indexOpcodeHandler0x1D;
    indexOpcodeHandlers[0x1E] = &CPUZ80::indexOpcodeHandler0x1E;
    indexOpcodeHandlers[0x21] = &CPUZ80::indexOpcodeHandler0x21;
    indexOpcodeHandlers[0x22] = &CPUZ80::indexOpcodeHandler0x22;
    indexOpcodeHandlers[0x23] = &CPUZ80::indexOpcodeHandler0x23;
    indexOpcodeHandlers[0x24] = &CPUZ80::indexOpcodeHandler0x24;
    indexOpcodeHandlers[0x25] = &CPUZ80::indexOpcodeHandler0x25;
    indexOpcodeHandlers[0x26] = &CPUZ80::indexOpcodeHandler0x26;
    indexOpcodeHandlers[0x29] = &CPUZ80::indexOpcodeHandler0x29;
    indexOpcodeHandlers[0x2A] = &CPUZ80::indexOpcodeHandler0x2A;
    indexOpcodeHandlers[0x2B] = &CPUZ80::indexOpcodeHandler0x2B;
    indexOpcodeHandlers[0x2C] = &CPUZ80::indexOpcodeHandler0x2C;
    indexOpcodeHandlers[0x2D] = &CPUZ80::indexOpcodeHandler0x2D;
    indexOpcodeHandlers[0x2E] = &CPUZ80::indexOpcodeHandler0x2E;
    indexOpcodeHandlers[0x34] = &CPUZ80::indexOpcodeHandler0x34;
    indexOpcodeHandlers[0x35] = &CPUZ80::indexOpcodeHandler0x35;
    indexOpcodeHandlers[0x36] = &CPUZ80::indexOpcodeHandler0x36;
    indexOpcodeHandlers[0x39] = &CPUZ80::indexOpcodeHandler0x39;
    indexOpcodeHandlers[0x3C] = &CPUZ80::indexOpcodeHandler0x3C;
    indexOpcodeHandlers[0x3D] = &CPUZ80::indexOpcodeHandler0x3D;
    indexOpcodeHandlers[0x3E] = &CPUZ80::indexOpcodeHandler0x3E;
    indexOpcodeHandlers[0x40] = &CPUZ80::indexOpcodeHandler0x40;
    indexOpcodeHandlers[0x41] = &CPUZ80::indexOpcodeHandler0x41;
    indexOpcodeHandlers[0x42] = &CPUZ80::indexOpcodeHandler0x42;
    indexOpcodeHandlers[0x43] = &CPUZ80::indexOpcodeHandler0x43;
    indexOpcodeHandlers[0x44] = &CPUZ80::indexOpcodeHandler0x44;
    indexOpcodeHandlers[0x45] = &CPUZ80::indexOpcodeHandler0x45;
    indexOpcodeHandlers[0x46] = &CPUZ80::indexOpcodeHandler0x46;
    indexOpcodeHandlers[0x47] = &CPUZ80::indexOpcodeHandler0x47;
    indexOpcodeHandlers[0x48] = &CPUZ80::indexOpcodeHandler0x48;
    indexOpcodeHandlers[0x49] = &CPUZ80::indexOpcodeHandler0x49;
    indexOpcodeHandlers[0x4A] = &CPUZ80::indexOpcodeHandler0x4A;
    indexOpcodeHandlers[0x4B] = &CPUZ80::indexOpcodeHandler0x4B;
    indexOpcodeHandlers[0x4C] = &CPUZ80::indexOpcodeHandler0x4C;
    indexOpcodeHandlers[0x4D] = &CPUZ80::indexOpcodeHandler0x4D;
    indexOpcodeHandlers[0x4E] = &CPUZ80::indexOpcodeHandler0x4E;
    indexOpcodeHandlers[0x4F] = &CPUZ80::indexOpcodeHandler0x4F;
    indexOpcodeHandlers[0x50] = &CPUZ80::indexOpcodeHandler0x50;
    indexOpcodeHandlers[0x51] = &CPUZ80::indexOpcodeHandler0x51;
    indexOpcodeHandlers[0x52] = &CPUZ80::indexOpcodeHandler0x52;
    indexOpcodeHandlers[0x53] = &CPUZ80::indexOpcodeHandler0x53;
    indexOpcodeHandlers[0x54] = &CPUZ80::indexOpcodeHandler0x54;
    indexOpcodeHandlers[0x55] = &CPUZ80::indexOpcodeHandler0x55;
    indexOpcodeHandlers[0x56] = &CPUZ80::indexOpcodeHandler0x56;
    indexOpcodeHandlers[0x57] = &CPUZ80::indexOpcodeHandler0x57;
    indexOpcodeHandlers[0x58] = &CPUZ80::indexOpcodeHandler0x58;
    indexOpcodeHandlers[0x59] = &CPUZ80::indexOpcodeHandler0x59;
    indexOpcodeHandlers[0x5A] = &CPUZ80::indexOpcodeHandler0x5A;
    indexOpcodeHandlers[0x5B] = &CPUZ80::indexOpcodeHandler0x5B;
    indexOpcodeHandlers[0x5C] = &CPUZ80::indexOpcodeHandler0x5C;
    indexOpcodeHandlers[0x5D] = &CPUZ80::indexOpcodeHandler0x5D;
    indexOpcodeHandlers[0x5E] = &CPUZ80::indexOpcodeHandler0x5E;
    indexOpcodeHandlers[0x5F] = &CPUZ80::indexOpcodeHandler0x5F;
    indexOpcodeHandlers[0x60] = &CPUZ80::indexOpcodeHandler0x60;
    indexOpcodeHandlers[0x61] = &CPUZ80::indexOpcodeHandler0x61;
    indexOpcodeHandlers[0x62] = &CPUZ80::indexOpcodeHandler0x62;
    indexOpcodeHandlers[0x63] = &CPUZ80::indexOpcodeHandler0x63;
    indexOpcodeHandlers[0x64] = &CPUZ80::indexOpcodeHandler0x64;
    indexOpcodeHandlers[0x65] = &CPUZ80::indexOpcodeHandler0x65;
    indexOpcodeHandlers[0x66] = &CPUZ80::indexOpcodeHandler0x66;
    indexOpcodeHandlers[0x67] = &CPUZ80::indexOpcodeHandler0x67;
    indexOpcodeHandlers[0x68] = &CPUZ80::indexOpcodeHandler0x68;
    indexOpcodeHandlers[0x69] = &CPUZ80::indexOpcodeHandler0x69;
    indexOpcodeHandlers[0x6A] = &CPUZ80::indexOpcodeHandler0x6A;
    indexOpcodeHandlers[0x6B] = &CPUZ80::indexOpcodeHandler0x6B;
    indexOpcodeHandlers[0x6C] = &CPUZ80::indexOpcodeHandler0x6C;
    indexOpcodeHandlers[0x6D] = &CPUZ80::indexOpcodeHandler0x6D;
    indexOpcodeHandlers[0x6E] = &CPUZ80::indexOpcodeHandler0x6E;
    indexOpcodeHandlers[0x6F] = &CPUZ80::indexOpcodeHandler0x6F;
    indexOpcodeHandlers[0x70] = &CPUZ80::indexOpcodeHandler0x70;
    indexOpcodeHandlers[0x71] = &CPUZ80::indexOpcodeHandler0x71;
    indexOpcodeHandlers[0x72] = &CPUZ80::indexOpcodeHandler0x72;
    indexOpcodeHandlers[0x73] = &CPUZ80::indexOpcodeHandler0x73;
    indexOpcodeHandlers[0x74] = &CPUZ80::indexOpcodeHandler0x74;
    indexOpcodeHandlers[0x75] = &CPUZ80::indexOpcodeHandler0x75;
    indexOpcodeHandlers[0x77] = &CPUZ80::indexOpcodeHandler0x77;
    indexOpcodeHandlers[0x78] = &CPUZ80::indexOpcodeHandler0x78;
    indexOpcodeHandlers[0x79] = &CPUZ80::indexOpcodeHandler0x79;
    indexOpcodeHandlers[0x7A] = &CPUZ80::indexOpcodeHandler0x7A;
    indexOpcodeHandlers[0x7B] = &CPUZ80::indexOpcodeHandler0x7B;
    indexOpcodeHandlers[0x7C] = &CPUZ80::indexOpcodeHandler0x7C;
    indexOpcodeHandlers[0x7D] = &CPUZ80::indexOpcodeHandler0x7D;
    indexOpcodeHandlers[0x7E] = &CPUZ80::indexOpcodeHandler0x7E;
    indexOpcodeHandlers[0x7F] = &CPUZ80::indexOpcodeHandler0x7F;
    indexOpcodeHandlers[0x80] = &CPUZ80::indexOpcodeHandler0x80;
    indexOpcodeHandlers[0x81] = &CPUZ80::indexOpcodeHandler0x81;
    indexOpcodeHandlers[0x82] = &CPUZ80::indexOpcodeHandler0x82;
    indexOpcodeHandlers[0x83] = &CPUZ80::indexOpcodeHandler0x83;
    indexOpcodeHandlers[0x84] = &CPUZ80::indexOpcodeHandler0x84;
    indexOpcodeHandlers[0x85] = &CPUZ80::indexOpcodeHandler0x85;
    indexOpcodeHandlers[0x86] = &CPUZ80::indexOpcodeHandler0x86;
    indexOpcodeHandlers[0x87] = &CPUZ80::indexOpcodeHandler0x87;
    indexOpcodeHandlers[0x88] = &CPUZ80::indexOpcodeHandler0x88;
    indexOpcodeHandlers[0x89] = &CPUZ80::indexOpcodeHandler0x89;
    indexOpcodeHandlers[0x8A] = &CPUZ80::indexOpcodeHandler0x8A;
    indexOpcodeHandlers[0x8B] = &CPUZ80::indexOpcodeHandler0x8B;
    indexOpcodeHandlers[0x8C] = &CPUZ80::indexOpcodeHandler0x8C;
    indexOpcodeHandlers[0x8D] = &CPUZ80::indexOpcodeHandler0x8D;
    indexOpcodeHandlers[0x8E] = &CPUZ80::indexOpcodeHandler0x8E;
    indexOpcodeHandlers[0x8F] = &CPUZ80::indexOpcodeHandler0x8F;
    indexOpcodeHandlers[0x90] = &CPUZ80::indexOpcodeHandler0x90;
    indexOpcodeHandlers[0x91] = &CPUZ80::indexOpcodeHandler0x91;
    indexOpcodeHandlers[0x92] = &CPUZ80::indexOpcodeHandler0x92;
    indexOpcodeHandlers[0x93] = &CPUZ80::indexOpcodeHandler0x93;
    indexOpcodeHandlers[0x94] = &CPUZ80::indexOpcodeHandler0x94;
    indexOpcodeHandlers[0x95] = &CPUZ80::indexOpcodeHandler0x95;
    indexOpcodeHandlers[0x96] = &CPUZ80::indexOpcodeHandler0x96;
    indexOpcodeHandlers[0x97] = &CPUZ80::indexOpcodeHandler0x97;
    indexOpcodeHandlers[0x98] = &CPUZ80::indexOpcodeHandler0x98;
    indexOpcodeHandlers[0x99] = &CPUZ80::indexOpcodeHandler0x99;
    indexOpcodeHandlers[0x9A] = &CPUZ80::indexOpcodeHandler0x9A;
    indexOpcodeHandlers[0x9B] = &CPUZ80::indexOpcodeHandler0x9B;
    indexOpcodeHandlers[0x9C] = &CPUZ80::indexOpcodeHandler0x9C;
    indexOpcodeHandlers[0x9D] = &CPUZ80::indexOpcodeHandler0x9D;
    indexOpcodeHandlers[0x9E] = &CPUZ80::indexOpcodeHandler0x9E;
    indexOpcodeHandlers[0x9F] = &CPUZ80::indexOpcodeHandler0x9F;
    indexOpcodeHandlers[0xA0] = &CPUZ80::indexOpcodeHandler0xA0;
    indexOpcodeHandlers[0xA1] = &CPUZ80::indexOpcodeHandler0xA1;
    indexOpcodeHandlers[0xA2] = &CPUZ80::indexOpcodeHandler0xA2;
    indexOpcodeHandlers[0xA3] = &CPUZ80::indexOpcodeHandler0xA3;
    indexOpcodeHandlers[0xA4] = &CPUZ80::indexOpcodeHandler0xA4;
    indexOpcodeHandlers[0xA5] = &CPUZ80::indexOpcodeHandler0xA5;
    indexOpcodeHandlers[0xA6] = &CPUZ80::indexOpcodeHandler0xA6;
    indexOpcodeHandlers[0xA7] = &CPUZ80::indexOpcodeHandler0xA7;
    indexOpcodeHandlers[0xA8] = &CPUZ80::indexOpcodeHandler0xA8;
    indexOpcodeHandlers[0xA9] = &CPUZ80::indexOpcodeHandler0xA9;
    indexOpcodeHandlers[0xAA] = &CPUZ80::indexOpcodeHandler0xAA;
    indexOpcodeHandlers[0xAB] = &CPUZ80::indexOpcodeHandler0xAB;
    indexOpcodeHandlers[0xAC] = &CPUZ80::indexOpcodeHandler0xAC;
    indexOpcodeHandlers[0xAD] = &CPUZ80::indexOpcodeHandler0xAD;
    indexOpcodeHandlers[0xAE] = &CPUZ80::indexOpcodeHandler0xAE;
    indexOpcodeHandlers[0xAF] = &CPUZ80::indexOpcodeHandler0xAF;
    indexOpcodeHandlers[0xB0] = &CPUZ80::indexOpcodeHandler0xB0;
    indexOpcodeHandlers[0xB1] = &CPUZ80::indexOpcodeHandler0xB1;
    indexOpcodeHandlers[0xB2] = &CPUZ80::indexOpcodeHandler0xB2;
    indexOpcodeHandlers[0xB3] = &CPUZ80::indexOpcodeHandler0xB3;
    indexOpcodeHandlers[0xB4] = &CPUZ80::indexOpcodeHandler0xB4;
    indexOpcodeHandlers[0xB5] = &CPUZ80::indexOpcodeHandler0xB5;
    indexOpcodeHandlers[0xB6] = &CPUZ80::indexOpcodeHandler0xB6;
    indexOpcodeHandlers[0xB7] = &CPUZ80::indexOpcodeHandler0xB7;
    indexOpcodeHandlers[0xB8] = &CPUZ80::indexOpcodeHandler0xB8;
    indexOpcodeHandlers[0xB9] = &CPUZ80::indexOpcodeHandler0xB9;
    indexOpcodeHandlers[0xBA] = &CPUZ80::indexOpcodeHandler0xBA;
    indexOpcodeHandlers[0xBB] = &CPUZ80::indexOpcodeHandler0xBB;
    indexOpcodeHandlers[0xBC] = &CPUZ80::indexOpcodeHandler0xBC;
    indexOpcodeHandlers[0xBD] = &CPUZ80::indexOpcodeHandler0xBD;
    indexOpcodeHandlers[0xBE] = &CPUZ80::indexOpcodeHandler0xBE;
    indexOpcodeHandlers[0xBF] = &CPUZ80::indexOpcodeHandler0xBF;
    indexOpcodeHandlers[0xCB] = &CPUZ80::indexOpcodeHandler0xCB;
    indexOpcodeHandlers[0xE1] = &CPUZ80::indexOpcodeHandler0xE1;
    indexOpcodeHandlers[0xE3] = &CPUZ80::indexOpcodeHandler0xE3;
    indexOpcodeHandlers[0xE5] = &CPUZ80::indexOpcodeHandler0xE5;
    indexOpcodeHandlers[0xE9] = &CPUZ80::indexOpcodeHandler0xE9;
    indexOpcodeHandlers[0xF9] = &CPUZ80::indexOpcodeHandler0xF9;

    indexBitOpcodeHandlers[0x00] = &CPUZ80::indexBitOpcodeHandler0x00;
    indexBitOpcodeHandlers[0x01] = &CPUZ80::indexBitOpcodeHandler0x01;
    indexBitOpcodeHandlers[0x02] = &CPUZ80::indexBitOpcodeHandler0x02;
    indexBitOpcodeHandlers[0x03] = &CPUZ80::indexBitOpcodeHandler0x03;
    indexBitOpcodeHandlers[0x04] = &CPUZ80::indexBitOpcodeHandler0x04;
    indexBitOpcodeHandlers[0x05] = &CPUZ80::indexBitOpcodeHandler0x05;
    indexBitOpcodeHandlers[0x06] = &CPUZ80::indexBitOpcodeHandler0x06;
    indexBitOpcodeHandlers[0x07] = &CPUZ80::indexBitOpcodeHandler0x07;
    indexBitOpcodeHandlers[0x08] = &CPUZ80::indexBitOpcodeHandler0x08;
    indexBitOpcodeHandlers[0x09] = &CPUZ80::indexBitOpcodeHandler0x09;
    indexBitOpcodeHandlers[0x0A] = &CPUZ80::indexBitOpcodeHandler0x0A;
    indexBitOpcodeHandlers[0x0B] = &CPUZ80::indexBitOpcodeHandler0x0B;
    indexBitOpcodeHandlers[0x0C] = &CPUZ80::indexBitOpcodeHandler0x0C;
    indexBitOpcodeHandlers[0x0D] = &CPUZ80::indexBitOpcodeHandler0x0D;
    indexBitOpcodeHandlers[0x0E] = &CPUZ80::indexBitOpcodeHandler0x0E;
    indexBitOpcodeHandlers[0x0F] = &CPUZ80::indexBitOpcodeHandler0x0F;
    indexBitOpcodeHandlers[0x10] = &CPUZ80::indexBitOpcodeHandler0x10;
    indexBitOpcodeHandlers[0x11] = &CPUZ80::indexBitOpcodeHandler0x11;
    indexBitOpcodeHandlers[0x12] = &CPUZ80::indexBitOpcodeHandler0x12;
    indexBitOpcodeHandlers[0x13] = &CPUZ80::indexBitOpcodeHandler0x13;
    indexBitOpcodeHandlers[0x14] = &CPUZ80::indexBitOpcodeHandler0x14;
    indexBitOpcodeHandlers[0x15] = &CPUZ80::indexBitOpcodeHandler0x15;
    indexBitOpcodeHandlers[0x16] = &CPUZ80::indexBitOpcodeHandler0x16;
    indexBitOpcodeHandlers[0x17] = &CPUZ80::indexBitOpcodeHandler0x17;
    indexBitOpcodeHandlers[0x18] = &CPUZ80::indexBitOpcodeHandler0x18;
    indexBitOpcodeHandlers[0x19] = &CPUZ80::indexBitOpcodeHandler0x19;
    indexBitOpcodeHandlers[0x1A] = &CPUZ80::indexBitOpcodeHandler0x1A;
    indexBitOpcodeHandlers[0x1B] = &CPUZ80::indexBitOpcodeHandler0x1B;
    indexBitOpcodeHandlers[0x1C] = &CPUZ80::indexBitOpcodeHandler0x1C;
    indexBitOpcodeHandlers[0x1D] = &CPUZ80::indexBitOpcodeHandler0x1D;
    indexBitOpcodeHandlers[0x1E] = &CPUZ80::indexBitOpcodeHandler0x1E;
    indexBitOpcodeHandlers[0x1F] = &CPUZ80::indexBitOpcodeHandler0x1F;
    indexBitOpcodeHandlers[0x20] = &CPUZ80::indexBitOpcodeHandler0x20;
    indexBitOpcodeHandlers[0x21] = &CPUZ80::indexBitOpcodeHandler0x21;
    indexBitOpcodeHandlers[0x22] = &CPUZ80::indexBitOpcodeHandler0x22;
    indexBitOpcodeHandlers[0x23] = &CPUZ80::indexBitOpcodeHandler0x23;
    indexBitOpcodeHandlers[0x24] = &CPUZ80::indexBitOpcodeHandler0x24;
    indexBitOpcodeHandlers[0x25] = &CPUZ80::indexBitOpcodeHandler0x25;
    indexBitOpcodeHandlers[0x26] = &CPUZ80::indexBitOpcodeHandler0x26;
    indexBitOpcodeHandlers[0x27] = &CPUZ80::indexBitOpcodeHandler0x27;
    indexBitOpcodeHandlers[0x28] = &CPUZ80::indexBitOpcodeHandler0x28;
    indexBitOpcodeHandlers[0x29] = &CPUZ80::indexBitOpcodeHandler0x29;
    indexBitOpcodeHandlers[0x2A] = &CPUZ80::indexBitOpcodeHandler0x2A;
    indexBitOpcodeHandlers[0x2B] = &CPUZ80::indexBitOpcodeHandler0x2B;
    indexBitOpcodeHandlers[0x2C] = &CPUZ80::indexBitOpcodeHandler0x2C;
    indexBitOpcodeHandlers[0x2D] = &CPUZ80::indexBitOpcodeHandler0x2D;
    indexBitOpcodeHandlers[0x2E] = &CPUZ80::indexBitOpcodeHandler0x2E;
    indexBitOpcodeHandlers[0x2F] = &CPUZ80::indexBitOpcodeHandler0x2F;
    indexBitOpcodeHandlers[0x30] = &CPUZ80::indexBitOpcodeHandler0x30;
    indexBitOpcodeHandlers[0x31] = &CPUZ80::indexBitOpcodeHandler0x31;
    indexBitOpcodeHandlers[0x32] = &CPUZ80::indexBitOpcodeHandler0x32;
    indexBitOpcodeHandlers[0x33] = &CPUZ80::indexBitOpcodeHandler0x33;
    indexBitOpcodeHandlers[0x34] = &CPUZ80::indexBitOpcodeHandler0x34;
    indexBitOpcodeHandlers[0x35] = &CPUZ80::indexBitOpcodeHandler0x35;
    indexBitOpcodeHandlers[0x36] = &CPUZ80::indexBitOpcodeHandler0x36;
    indexBitOpcodeHandlers[0x37] = &CPUZ80::indexBitOpcodeHandler0x37;
    indexBitOpcodeHandlers[0x38] = &CPUZ80::indexBitOpcodeHandler0x38;
    indexBitOpcodeHandlers[0x39] = &CPUZ80::indexBitOpcodeHandler0x39;
    indexBitOpcodeHandlers[0x3A] = &CPUZ80::indexBitOpcodeHandler0x3A;
    indexBitOpcodeHandlers[0x3B] = &CPUZ80::indexBitOpcodeHandler0x3B;
    indexBitOpcodeHandlers[0x3C] = &CPUZ80::indexBitOpcodeHandler0x3C;
    indexBitOpcodeHandlers[0x3D] = &CPUZ80::indexBitOpcodeHandler0x3D;
    indexBitOpcodeHandlers[0x3E] = &CPUZ80::indexBitOpcodeHandler0x3E;
    indexBitOpcodeHandlers[0x3F] = &CPUZ80::indexBitOpcodeHandler0x3F;
    indexBitOpcodeHandlers[0x40] = &CPUZ80::indexBitOpcodeHandler0x40;
    indexBitOpcodeHandlers[0x41] = &CPUZ80::indexBitOpcodeHandler0x41;
    indexBitOpcodeHandlers[0x42] = &CPUZ80::indexBitOpcodeHandler0x42;
    indexBitOpcodeHandlers[0x43] = &CPUZ80::indexBitOpcodeHandler0x43;
    indexBitOpcodeHandlers[0x44] = &CPUZ80::indexBitOpcodeHandler0x44;
    indexBitOpcodeHandlers[0x45] = &CPUZ80::indexBitOpcodeHandler0x45;
    indexBitOpcodeHandlers[0x46] = &CPUZ80::indexBitOpcodeHandler0x46;
    indexBitOpcodeHandlers[0x47] = &CPUZ80::indexBitOpcodeHandler0x47;
    indexBitOpcodeHandlers[0x48] = &CPUZ80::indexBitOpcodeHandler0x48;
    indexBitOpcodeHandlers[0x49] = &CPUZ80::indexBitOpcodeHandler0x49;
    indexBitOpcodeHandlers[0x4A] = &CPUZ80::indexBitOpcodeHandler0x4A;
    indexBitOpcodeHandlers[0x4B] = &CPUZ80::indexBitOpcodeHandler0x4B;
    indexBitOpcodeHandlers[0x4C] = &CPUZ80::indexBitOpcodeHandler0x4C;
    indexBitOpcodeHandlers[0x4D] = &CPUZ80::indexBitOpcodeHandler0x4D;
    indexBitOpcodeHandlers[0x4E] = &CPUZ80::indexBitOpcodeHandler0x4E;
    indexBitOpcodeHandlers[0x4F] = &CPUZ80::indexBitOpcodeHandler0x4F;
    indexBitOpcodeHandlers[0x50] = &CPUZ80::indexBitOpcodeHandler0x50;
    indexBitOpcodeHandlers[0x51] = &CPUZ80::indexBitOpcodeHandler0x51;
    indexBitOpcodeHandlers[0x52] = &CPUZ80::indexBitOpcodeHandler0x52;
    indexBitOpcodeHandlers[0x53] = &CPUZ80::indexBitOpcodeHandler0x53;
    indexBitOpcodeHandlers[0x54] = &CPUZ80::indexBitOpcodeHandler0x54;
    indexBitOpcodeHandlers[0x55] = &CPUZ80::indexBitOpcodeHandler0x55;
    indexBitOpcodeHandlers[0x56] = &CPUZ80::indexBitOpcodeHandler0x56;
    indexBitOpcodeHandlers[0x57] = &CPUZ80::indexBitOpcodeHandler0x57;
    indexBitOpcodeHandlers[0x58] = &CPUZ80::indexBitOpcodeHandler0x58;
    indexBitOpcodeHandlers[0x59] = &CPUZ80::indexBitOpcodeHandler0x59;
    indexBitOpcodeHandlers[0x5A] = &CPUZ80::indexBitOpcodeHandler0x5A;
    indexBitOpcodeHandlers[0x5B] = &CPUZ80::indexBitOpcodeHandler0x5B;
    indexBitOpcodeHandlers[0x5C] = &CPUZ80::indexBitOpcodeHandler0x5C;
    indexBitOpcodeHandlers[0x5D] = &CPUZ80::indexBitOpcodeHandler0x5D;
    indexBitOpcodeHandlers[0x5E] = &CPUZ80::indexBitOpcodeHandler0x5E;
    indexBitOpcodeHandlers[0x5F] = &CPUZ80::indexBitOpcodeHandler0x5F;
    indexBitOpcodeHandlers[0x60] = &CPUZ80::indexBitOpcodeHandler0x60;
    indexBitOpcodeHandlers[0x61] = &CPUZ80::indexBitOpcodeHandler0x61;
    indexBitOpcodeHandlers[0x62] = &CPUZ80::indexBitOpcodeHandler0x62;
    indexBitOpcodeHandlers[0x63] = &CPUZ80::indexBitOpcodeHandler0x63;
    indexBitOpcodeHandlers[0x64] = &CPUZ80::indexBitOpcodeHandler0x64;
    indexBitOpcodeHandlers[0x65] = &CPUZ80::indexBitOpcodeHandler0x65;
    indexBitOpcodeHandlers[0x66] = &CPUZ80::indexBitOpcodeHandler0x66;
    indexBitOpcodeHandlers[0x67] = &CPUZ80::indexBitOpcodeHandler0x67;
    indexBitOpcodeHandlers[0x68] = &CPUZ80::indexBitOpcodeHandler0x68;
    indexBitOpcodeHandlers[0x69] = &CPUZ80::indexBitOpcodeHandler0x69;
    indexBitOpcodeHandlers[0x6A] = &CPUZ80::indexBitOpcodeHandler0x6A;
    indexBitOpcodeHandlers[0x6B] = &CPUZ80::indexBitOpcodeHandler0x6B;
    indexBitOpcodeHandlers[0x6C] = &CPUZ80::indexBitOpcodeHandler0x6C;
    indexBitOpcodeHandlers[0x6D] = &CPUZ80::indexBitOpcodeHandler0x6D;
    indexBitOpcodeHandlers[0x6E] = &CPUZ80::indexBitOpcodeHandler0x6E;
    indexBitOpcodeHandlers[0x6F] = &CPUZ80::indexBitOpcodeHandler0x6F;
    indexBitOpcodeHandlers[0x70] = &CPUZ80::indexBitOpcodeHandler0x70;
    indexBitOpcodeHandlers[0x71] = &CPUZ80::indexBitOpcodeHandler0x71;
    indexBitOpcodeHandlers[0x72] = &CPUZ80::indexBitOpcodeHandler0x72;
    indexBitOpcodeHandlers[0x73] = &CPUZ80::indexBitOpcodeHandler0x73;
    indexBitOpcodeHandlers[0x74] = &CPUZ80::indexBitOpcodeHandler0x74;
    indexBitOpcodeHandlers[0x75] = &CPUZ80::indexBitOpcodeHandler0x75;
    indexBitOpcodeHandlers[0x76] = &CPUZ80::indexBitOpcodeHandler0x76;
    indexBitOpcodeHandlers[0x77] = &CPUZ80::indexBitOpcodeHandler0x77;
    indexBitOpcodeHandlers[0x78] = &CPUZ80::indexBitOpcodeHandler0x78;
    indexBitOpcodeHandlers[0x79] = &CPUZ80::indexBitOpcodeHandler0x79;
    indexBitOpcodeHandlers[0x7A] = &CPUZ80::indexBitOpcodeHandler0x7A;
    indexBitOpcodeHandlers[0x7B] = &CPUZ80::indexBitOpcodeHandler0x7B;
    indexBitOpcodeHandlers[0x7C] = &CPUZ80::indexBitOpcodeHandler0x7C;
    indexBitOpcodeHandlers[0x7D] = &CPUZ80::indexBitOpcodeHandler0x7D;
    indexBitOpcodeHandlers[0x7E] = &CPUZ80::indexBitOpcodeHandler0x7E;
    indexBitOpcodeHandlers[0x7F] = &CPUZ80::indexBitOpcodeHandler0x7F;
    indexBitOpcodeHandlers[0x80] = &CPUZ80::indexBitOpcodeHandler0x80;
    indexBitOpcodeHandlers[0x81] = &CPUZ80::indexBitOpcodeHandler0x81;
    indexBitOpcodeHandlers[0x82] = &CPUZ80::indexBitOpcodeHandler0x82;
    indexBitOpcodeHandlers[0x83] = &CPUZ80::indexBitOpcodeHandler0x83;
    indexBitOpcodeHandlers[0x84] = &CPUZ80::indexBitOpcodeHandler0x84;
    indexBitOpcodeHandlers[0x85] = &CPUZ80::indexBitOpcodeHandler0x85;
    indexBitOpcodeHandlers[0x86] = &CPUZ80::indexBitOpcodeHandler0x86;
    indexBitOpcodeHandlers[0x87] = &CPUZ80::indexBitOpcodeHandler0x87;
    indexBitOpcodeHandlers[0x88] = &CPUZ80::indexBitOpcodeHandler0x88;
    indexBitOpcodeHandlers[0x89] = &CPUZ80::indexBitOpcodeHandler0x89;
    indexBitOpcodeHandlers[0x8A] = &CPUZ80::indexBitOpcodeHandler0x8A;
    indexBitOpcodeHandlers[0x8B] = &CPUZ80::indexBitOpcodeHandler0x8B;
    indexBitOpcodeHandlers[0x8C] = &CPUZ80::indexBitOpcodeHandler0x8C;
    indexBitOpcodeHandlers[0x8D] = &CPUZ80::indexBitOpcodeHandler0x8D;
    indexBitOpcodeHandlers[0x8E] = &CPUZ80::indexBitOpcodeHandler0x8E;
    indexBitOpcodeHandlers[0x8F] = &CPUZ80::indexBitOpcodeHandler0x8F;
    indexBitOpcodeHandlers[0x90] = &CPUZ80::indexBitOpcodeHandler0x90;
    indexBitOpcodeHandlers[0x91] = &CPUZ80::indexBitOpcodeHandler0x91;
    indexBitOpcodeHandlers[0x92] = &CPUZ80::indexBitOpcodeHandler0x92;
    indexBitOpcodeHandlers[0x93] = &CPUZ80::indexBitOpcodeHandler0x93;
    indexBitOpcodeHandlers[0x94] = &CPUZ80::indexBitOpcodeHandler0x94;
    indexBitOpcodeHandlers[0x95] = &CPUZ80::indexBitOpcodeHandler0x95;
    indexBitOpcodeHandlers[0x96] = &CPUZ80::indexBitOpcodeHandler0x96;
    indexBitOpcodeHandlers[0x97] = &CPUZ80::indexBitOpcodeHandler0x97;
    indexBitOpcodeHandlers[0x98] = &CPUZ80::indexBitOpcodeHandler0x98;
    indexBitOpcodeHandlers[0x99] = &CPUZ80::indexBitOpcodeHandler0x99;
    indexBitOpcodeHandlers[0x9A] = &CPUZ80::indexBitOpcodeHandler0x9A;
    indexBitOpcodeHandlers[0x9B] = &CPUZ80::indexBitOpcodeHandler0x9B;
    indexBitOpcodeHandlers[0x9C] = &CPUZ80::indexBitOpcodeHandler0x9C;
    indexBitOpcodeHandlers[0x9D] = &CPUZ80::indexBitOpcodeHandler0x9D;
    indexBitOpcodeHandlers[0x9E] = &CPUZ80::indexBitOpcodeHandler0x9E;
    indexBitOpcodeHandlers[0x9F] = &CPUZ80::indexBitOpcodeHandler0x9F;
    indexBitOpcodeHandlers[0xA0] = &CPUZ80::indexBitOpcodeHandler0xA0;
    indexBitOpcodeHandlers[0xA1] = &CPUZ80::indexBitOpcodeHandler0xA1;
    indexBitOpcodeHandlers[0xA2] = &CPUZ80::indexBitOpcodeHandler0xA2;
    indexBitOpcodeHandlers[0xA3] = &CPUZ80::indexBitOpcodeHandler0xA3;
    indexBitOpcodeHandlers[0xA4] = &CPUZ80::indexBitOpcodeHandler0xA4;
    indexBitOpcodeHandlers[0xA5] = &CPUZ80::indexBitOpcodeHandler0xA5;
    indexBitOpcodeHandlers[0xA6] = &CPUZ80::indexBitOpcodeHandler0xA6;
    indexBitOpcodeHandlers[0xA7] = &CPUZ80::indexBitOpcodeHandler0xA7;
    indexBitOpcodeHandlers[0xA8] = &CPUZ80::indexBitOpcodeHandler0xA8;
    indexBitOpcodeHandlers[0xA9] = &CPUZ80::indexBitOpcodeHandler0xA9;
    indexBitOpcodeHandlers[0xAA] = &CPUZ80::indexBitOpcodeHandler0xAA;
    indexBitOpcodeHandlers[0xAB] = &CPUZ80::indexBitOpcodeHandler0xAB;
    indexBitOpcodeHandlers[0xAC] = &CPUZ80::indexBitOpcodeHandler0xAC;
    indexBitOpcodeHandlers[0xAD] = &CPUZ80::indexBitOpcodeHandler0xAD;
    indexBitOpcodeHandlers[0xAE] = &CPUZ80::indexBitOpcodeHandler0xAE;
    indexBitOpcodeHandlers[0xAF] = &CPUZ80::indexBitOpcodeHandler0xAF;
    indexBitOpcodeHandlers[0xB0] = &CPUZ80::indexBitOpcodeHandler0xB0;
    indexBitOpcodeHandlers[0xB1] = &CPUZ80::indexBitOpcodeHandler0xB1;
    indexBitOpcodeHandlers[0xB2] = &CPUZ80::indexBitOpcodeHandler0xB2;
    indexBitOpcodeHandlers[0xB3] = &CPUZ80::indexBitOpcodeHandler0xB3;
    indexBitOpcodeHandlers[0xB4] = &CPUZ80::indexBitOpcodeHandler0xB4;
    indexBitOpcodeHandlers[0xB5] = &CPUZ80::indexBitOpcodeHandler0xB5;
    indexBitOpcodeHandlers[0xB6] = &CPUZ80::indexBitOpcodeHandler0xB6;
    indexBitOpcodeHandlers[0xB7] = &CPUZ80::indexBitOpcodeHandler0xB7;
    indexBitOpcodeHandlers[0xB8] = &CPUZ80::indexBitOpcodeHandler0xB8;
    indexBitOpcodeHandlers[0xB9] = &CPUZ80::indexBitOpcodeHandler0xB9;
    indexBitOpcodeHandlers[0xBA] = &CPUZ80::indexBitOpcodeHandler0xBA;
    indexBitOpcodeHandlers[0xBB] = &CPUZ80::indexBitOpcodeHandler0xBB;
    indexBitOpcodeHandlers[0xBC] = &CPUZ80::indexBitOpcodeHandler0xBC;
    indexBitOpcodeHandlers[0xBD] = &CPUZ80::indexBitOpcodeHandler0xBD;
    indexBitOpcodeHandlers[0xBE] = &CPUZ80::indexBitOpcodeHandler0xBE;
    indexBitOpcodeHandlers[0xBF] = &CPUZ80::indexBitOpcodeHandler0xBF;
    indexBitOpcodeHandlers[0xC0] = &CPUZ80::indexBitOpcodeHandler0xC0;
    indexBitOpcodeHandlers[0xC1] = &CPUZ80::indexBitOpcodeHandler0xC1;
    indexBitOpcodeHandlers[0xC2] = &CPUZ80::indexBitOpcodeHandler0xC2;
    indexBitOpcodeHandlers[0xC3] = &CPUZ80::indexBitOpcodeHandler0xC3;
    indexBitOpcodeHandlers[0xC4] = &CPUZ80::indexBitOpcodeHandler0xC4;
    indexBitOpcodeHandlers[0xC5] = &CPUZ80::indexBitOpcodeHandler0xC5;
    indexBitOpcodeHandlers[0xC6] = &CPUZ80::indexBitOpcodeHandler0xC6;
    indexBitOpcodeHandlers[0xC7] = &CPUZ80::indexBitOpcodeHandler0xC7;
    indexBitOpcodeHandlers[0xC8] = &CPUZ80::indexBitOpcodeHandler0xC8;
    indexBitOpcodeHandlers[0xC9] = &CPUZ80::indexBitOpcodeHandler0xC9;
    indexBitOpcodeHandlers[0xCA] = &CPUZ80::indexBitOpcodeHandler0xCA;
    indexBitOpcodeHandlers[0xCB] = &CPUZ80::indexBitOpcodeHandler0xCB;
    indexBitOpcodeHandlers[0xCC] = &CPUZ80::indexBitOpcodeHandler0xCC;
    indexBitOpcodeHandlers[0xCD] = &CPUZ80::indexBitOpcodeHandler0xCD;
    indexBitOpcodeHandlers[0xCE] = &CPUZ80::indexBitOpcodeHandler0xCE;
    indexBitOpcodeHandlers[0xCF] = &CPUZ80::indexBitOpcodeHandler0xCF;
    indexBitOpcodeHandlers[0xD0] = &CPUZ80::indexBitOpcodeHandler0xD0;
    indexBitOpcodeHandlers[0xD1] = &CPUZ80::indexBitOpcodeHandler0xD1;
    indexBitOpcodeHandlers[0xD2] = &CPUZ80::indexBitOpcodeHandler0xD2;
    indexBitOpcodeHandlers[0xD3] = &CPUZ80::indexBitOpcodeHandler0xD3;
    indexBitOpcodeHandlers[0xD4] = &CPUZ80::indexBitOpcodeHandler0xD4;
    indexBitOpcodeHandlers[0xD5] = &CPUZ80::indexBitOpcodeHandler0xD5;
    indexBitOpcodeHandlers[0xD6] = &CPUZ80::indexBitOpcodeHandler0xD6;
    indexBitOpcodeHandlers[0xD7] = &CPUZ80::indexBitOpcodeHandler0xD7;
    indexBitOpcodeHandlers[0xD8] = &CPUZ80::indexBitOpcodeHandler0xD8;
    indexBitOpcodeHandlers[0xD9] = &CPUZ80::indexBitOpcodeHandler0xD9;
    indexBitOpcodeHandlers[0xDA] = &CPUZ80::indexBitOpcodeHandler0xDA;
    indexBitOpcodeHandlers[0xDB] = &CPUZ80::indexBitOpcodeHandler0xDB;
    indexBitOpcodeHandlers[0xDC] = &CPUZ80::indexBitOpcodeHandler0xDC;
    indexBitOpcodeHandlers[0xDD] = &CPUZ80::indexBitOpcodeHandler0xDD;
    indexBitOpcodeHandlers[0xDE] = &CPUZ80::indexBitOpcodeHandler0xDE;
    indexBitOpcodeHandlers[0xDF] = &CPUZ80::indexBitOpcodeHandler0xDF;
    indexBitOpcodeHandlers[0xE0] = &CPUZ80::indexBitOpcodeHandler0xE0;
    indexBitOpcodeHandlers[0xE1] = &CPUZ80::indexBitOpcodeHandler0xE1;
    indexBitOpcodeHandlers[0xE2] = &CPUZ80::indexBitOpcodeHandler0xE2;
    indexBitOpcodeHandlers[0xE3] = &CPUZ80::indexBitOpcodeHandler0xE3;
    indexBitOpcodeHandlers[0xE4] = &CPUZ80::indexBitOpcodeHandler0xE4;
    indexBitOpcodeHandlers[0xE5] = &CPUZ80::indexBitOpcodeHandler0xE5;
    indexBitOpcodeHandlers[0xE6] = &CPUZ80::indexBitOpcodeHandler0xE6;
    indexBitOpcodeHandlers[0xE7] = &CPUZ80::indexBitOpcodeHandler0xE7;
    indexBitOpcodeHandlers[0xE8] = &CPUZ80::indexBitOpcodeHandler0xE8;
    indexBitOpcodeHandlers[0xE9] = &CPUZ80::indexBitOpcodeHandler0xE9;
    indexBitOpcodeHandlers[0xEA] = &CPUZ80::indexBitOpcodeHandler0xEA;
    indexBitOpcodeHandlers[0xEB] = &CPUZ80::indexBitOpcodeHandler0xEB;
    indexBitOpcodeHandlers[0xEC] = &CPUZ80::indexBitOpcodeHandler0xEC;
    indexBitOpcodeHandlers[0xED] = &CPUZ80::indexBitOpcodeHandler0xED;
    indexBitOpcodeHandlers[0xEE] = &CPUZ80::indexBitOpcodeHandler0xEE;
    indexBitOpcodeHandlers[0xEF] = &CPUZ80::indexBitOpcodeHandler0xEF;
    indexBitOpcodeHandlers[0xF0] = &CPUZ80::indexBitOpcodeHandler0xF0;
    indexBitOpcodeHandlers[0xF1] = &CPUZ80::indexBitOpcodeHandler0xF1;
    indexBitOpcodeHandlers[0xF2] = &CPUZ80::indexBitOpcodeHandler0xF2;
    indexBitOpcodeHandlers[0xF3] = &CPUZ80::indexBitOpcodeHandler0xF3;
    indexBitOpcodeHandlers[0xF4] = &CPUZ80::indexBitOpcodeHandler0xF4;
    indexBitOpcodeHandlers[0xF5] = &CPUZ80::indexBitOpcodeHandler0xF5;
    indexBitOpcodeHandlers[0xF6] = &CPUZ80::indexBitOpcodeHandler0xF6;
    indexBitOpcodeHandlers[0xF7] = &CPUZ80::indexBitOpcodeHandler0xF7;
    indexBitOpcodeHandlers[0xF8] = &CPUZ80::indexBitOpcodeHandler0xF8;
    indexBitOpcodeHandlers[0xF9] = &CPUZ80::indexBitOpcodeHandler0xF9;
    indexBitOpcodeHandlers[0xFA] = &CPUZ80::indexBitOpcodeHandler0xFA;
    indexBitOpcodeHandlers[0xFB] = &CPUZ80::indexBitOpcodeHandler0xFB;
    indexBitOpcodeHandlers[0xFC] = &CPUZ80::indexBitOpcodeHandler0xFC;
    indexBitOpcodeHandlers[0xFD] = &CPUZ80::indexBitOpcodeHandler0xFD;
    indexBitOpcodeHandlers[0xFE] = &CPUZ80::indexBitOpcodeHandler0xFE;
    indexBitOpcodeHandlers[0xFF] = &CPUZ80::indexBitOpcodeHandler0xFF;
}