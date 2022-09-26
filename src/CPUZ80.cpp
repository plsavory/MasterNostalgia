/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#define VERBOSE_MODE

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"

CPUZ80::CPUZ80(Memory *smsMemory) {
    // Store a pointer to the memory object
    memory = smsMemory;

    cyclesTaken = 0;

    // Reset the CPU to its initial state
    reset();
}

CPUZ80::~CPUZ80() {

}

void CPUZ80::reset() {
#ifdef VERBOSE_MODE
    std::cout << "Resetting CPU..." << std::endl;
#endif

    originalProgramCounterValue = programCounter = 0x0;
    stackPointer = 0xDFF0;
    iff1 = iff2 = true;
    enableInterrupts = false;

    for (auto & gpRegister : gpRegisters) {
        gpRegister.whole = 0x0;
    }

    state = CPUState::Running;
}

int CPUZ80::execute() {
    // This function may be redundant - TODO: Call executeOpcode directly from MasterSystem class if this turns out to be the case in the future.
    if (state == CPUState::Running) {
        return executeOpcode();
    }

    return 0;
}

int CPUZ80::executeOpcode() {
    originalProgramCounterValue = programCounter;
    unsigned char opcode = NB();
    unsigned char upperOpcode; // Use for 2-byte instructions
    cyclesTaken = 0;

#ifdef VERBOSE_MODE
    std::string prefix = "";
    unsigned char displayOpcode = opcode;
#endif

    if (enableInterrupts) {
        iff1 = iff2 = true;
        enableInterrupts = false;
    }

    switch (opcode) {
        case 0x0:
            // nop
            cyclesTaken = 4;
            break;
        case 0x03:
            // inc bc
            inc16Bit(gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 6;
            break;
        case 0x13:
            // inc de
            inc16Bit(gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 6;
            break;
        case 0x23:
            // inc hl
            inc16Bit(gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 6;
            break;
        case 0x31:
            // ld sp, nn
            ldReg16(stackPointer, build16BitNumber(), false);
            cyclesTaken = 10;
            break;
        case 0x32:
            // ld (nn), a
            memory->write(build16BitNumber(), gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 13;
            break;
        case 0x33:
            // inc sp
            inc16Bit(stackPointer);
            cyclesTaken = 6;
            break;
        case 0x3C:
            // inc a
            inc8Bit(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
             break;
        case 0x3E:
            // ld a, n
            ldReg8(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0x43:
            // ld b, e
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x53:
            // ld d, e
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].lo);
            break;
        case 0x63:
            // ld h, e
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::DE].lo);
            break;
        case 0x73:
            // ld (hl), e
            memory->write(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 7;
            break;
        case 0x83:
            // add a, e
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x93:
            // sub e
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0xA3:
            // and e
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0xB3:
            // or e
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0xC2:
            jpCondition(JPCondition::NZ, NB());
            cyclesTaken = 10;
            break;
        case 0xC3:
            // jp nn
            jpImm();
            cyclesTaken = 10;
            break;
        case 0xDB:
            // in a, (n)
            ldReg8(gpRegisters[cpuReg::AF].hi, readIOPort(NB()));
            cyclesTaken = 11;
            break;
        case 0xED:
            upperOpcode = NB();
            extendedOpcodes(upperOpcode);

#ifdef VERBOSE_MODE
            prefix = "ED";
            displayOpcode = upperOpcode;
#endif

            break;
        case 0xF3:
            // di
            iff1 = iff2 = false;
            cyclesTaken = 4;
            break;
        case 0xFB:
            // ei
            enableInterrupts = true; // Interrupts should be re-enabled when executing the next instruction
            cyclesTaken = 4;
            break;
        default:
            state = CPUState::Error;

            std::cout << "Error: Unknown opcode: 0x" << std::hex << (int) opcode << "-  At PC: 0x"
                      << (int) originalProgramCounterValue << std::endl;
            break;
    }

#ifdef VERBOSE_MODE
    if (state != CPUState::Error)
        logCPUState(displayOpcode, prefix);
#endif

    return cyclesTaken;
}

/**
 * [CPUZ80::NB Fetch the next byte in memory]
 * @return [description]
 */
unsigned char CPUZ80::NB() {
    return memory->read(programCounter++);
}

/**
 * [CPUZ80::extendedOpcodes Handles extended opcodes]
 */
void CPUZ80::extendedOpcodes(unsigned char opcode) {

    switch (opcode) {
        case 0x46:
            // im 0
            setInterruptMode(0);
            break;
        case 0x56:
            // im 1
            setInterruptMode(1);
            break;
        case 0x5E:
            setInterruptMode(2);
            break;
        default:
            state = CPUState::Error;

#ifdef VERBOSE_MODE
            std::cout << "Error: Unknown extended opcode: 0x" << std::hex << (int) opcode << std::endl;
#endif

            break;
    }
}

/**
 * [logCPUState Log the CPU's current state to the console]
 */
void CPUZ80::logCPUState(unsigned char opcode, std::string prefix) {
    std::cout << std::uppercase << "PC: 0x" << std::hex << (int) originalProgramCounterValue << " Opcode: 0x" << prefix
              << (int) opcode << " Registers: AF=0x" << (int) gpRegisters[cpuReg::AF].whole << " BC=0x"
              << (int) gpRegisters[cpuReg::BC].whole << " DE=0x" << (int) gpRegisters[cpuReg::DE].whole << " HL=0x"
              << (int) gpRegisters[cpuReg::HL].whole << " IX=0x" << (int) gpRegisters[cpuReg::rIX].whole << " IY=0x"
              << (int) gpRegisters[cpuReg::rIY].whole << " SP=0x" << (int) stackPointer << std::endl;
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
    unsigned char addrLo = NB();
    unsigned char addrHi = NB();

    return (addrLo + (addrHi << 8));
}

/**
 * [CPUZ80::get16BitRelativeValue Returns the value stored within the given memory address]
 * @return [description]
 */
unsigned char CPUZ80::getIndirectValue() {
    return memory->read(build16BitNumber());
}

/**
 * [CPUZ80::get16BitRelativeValue Returns the value stored within the given memory address]
 * @return [description]
 */
unsigned char CPUZ80::getIndirectValue(unsigned short address) {
    return memory->read(address);
}

void CPUZ80::writeIOPort(unsigned char address, unsigned char value) {

    // TODO handle the Game Gear I/O port map if I ever add support for it

    if (address <= 0x3F) {
        // TODO even address - write to memory control register
        // TODO odd address - write to I/O control register
        return;
    }

    if (address <= 0x7F) {
        // TODO - write to SN76489 PSG
        return;
    }

    if (address <= 0xBF) {
        // TODO even address - write to VDP data port
        // TODO odd address - write to VDP control port
        return;
    }

    // TODO even address - return I/O port A/B register
    // TODO odd address - return I/O port B/misc register

}

unsigned char CPUZ80::readIOPort(unsigned char address) {

    // TODO handle the Game Gear I/O port map if I ever add support for it

    if (address <= 0x3F) {
        // Return the last byte of the instruction which read the port
        // TODO or 0xFF if emulating Master System 2
        return address;
    }

    if (address <= 0x7F) {
        // TODO even address - return SN76489 PSG V counter
        // TODO odd address - return SN76489 PSG H counter
        return 0x0;
    }

    if (address <= 0xBF) {
        // TODO even address - return VDP data port contents
        // TODO odd address - return VDP status flags
        return 0x0;
    }

    // TODO even address - return I/O port A/B register
    // TODO odd address - return I/O port B/misc register
    return 0x0;

}