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

    // Reset the CPU to its initial state
    reset();
}

CPUZ80::~CPUZ80() {

}

void CPUZ80::reset() {
#ifdef VERBOSE_MODE
    std::cout << "Resetting CPU..." << std::endl;
#endif

    programCounter = 0x0; // TODO: Give this the real value of the entry point of the Z80 binaries when I actually learn what that is.
    stackPointer = 0xDFF0;

    for (int i = 0; i < 10; i++) {
        gpRegisters[i].whole = 0x0;
    }

    state = cpuState::Running;
}

int CPUZ80::execute() {
    // This function may be redundant - TODO: Call executeOpcode directly from MasterSystem class if this turns out to be the case in the future.
    if (state == cpuState::Running) {
        executeOpcode();
    }

    return 0;
}

void CPUZ80::executeOpcode() {
    unsigned char opcode = NB();
    unsigned char upperOpcode; // Use for 2-byte instructions

#ifdef VERBOSE_MODE
    std::string prefix = "";
    unsigned char displayOpcode = opcode;
#endif

    switch (opcode) {
        case 0xAE: // XOR (hl)
            break;
        case 0xC3: // JP (nn)
            jpImm();
            break;
        case 0xC2:
            jpCondition(JPCondition::NZ, NB());
            break;
        case 0xED:
            upperOpcode = NB();
            extendedOpcodes(upperOpcode);

#ifdef VERBOSE_MODE
            prefix = "ED";
            displayOpcode = upperOpcode;
#endif

            break;
        default:
            state = cpuState::Error;

#ifdef VERBOSE_MODE
            std::cout << "Error: Unknown opcode: 0x" << std::hex << (int) opcode << "-  At PC: 0x"
                      << (int) programCounter << std::endl;
#endif

            break;
    }

#ifdef VERBOSE_MODE
    if (state != cpuState::Error)
        logCPUState(displayOpcode, prefix);
#endif
}

/**
 * [CPUZ80::NB Fetch the next byte in memory]
 * @return [description]
 */
unsigned char CPUZ80::NB() {
    return memory->read(++programCounter);
}

/**
 * [CPUZ80::extendedOpcodes Handles extended opcodes]
 */
void CPUZ80::extendedOpcodes(unsigned char opcode) {

    switch (opcode) {
        case 0x46:
            setInterruptMode(0);
            break;
        case 0x4E:
            setInterruptMode(
                    0); // TODO: Confirm whether this is correct, seems to be an instruction with undefined behaviour.
            break;
        case 0x56:
            setInterruptMode(1);
            break;
        case 0x5E:
            setInterruptMode(2);
            break;
        case 0x66:
            setInterruptMode(0);
            break;
        case 0x6E:
            setInterruptMode(
                    0); // TODO: Confirm whether this is correct, seems to be an instruction with undefined behaviour.
            break;
        case 0x76:
            setInterruptMode(1);
            break;
        case 0x7E:
            setInterruptMode(2);
            break;
        default:
            state = cpuState::Error;

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
    std::cout << std::uppercase << "PC: 0x" << std::hex << (int) programCounter << " Opcode: 0x" << prefix
              << (int) opcode << " Registers: AF=0x" << (int) gpRegisters[cpuReg::AF].whole << " BC=0x"
              << (int) gpRegisters[cpuReg::BC].whole << " DE=0x" << (int) gpRegisters[cpuReg::DE].whole << " HL=0x"
              << (int) gpRegisters[cpuReg::HL].whole << " IX=0x" << (int) gpRegisters[cpuReg::rIX].whole << " IY=0x"
              << (int) gpRegisters[cpuReg::rIY].whole << std::endl;
}

/**
 * [CPUZ80::setFlag Sets a flag - Definitely redundant... just saves me some extra typing]
 * @param flag  [The flag to set a value on]
 * @param value [The value to set on the flag]
 */
void CPUZ80::setFlag(CPUFlag flag, bool value) {
    Utils::setBit(flag, value, gpRegisters[cpuReg::AF].lo);
}

/**
 * [CPUZ80::getFlag Gets a flag - Definitely redundant... just saves me some extra typing]
 * @param  flag [The flag to get the value of]
 * @return      [The value of the flag]
 */
bool CPUZ80::getFlag(CPUFlag flag) {
    return Utils::testBit(flag, gpRegisters[cpuReg::AF].lo);
}

/**
 * [CPUZ80::build16BitAddress When called, increments the pc by 4 and builds a 16-bit memory address]
 * @return [The memory address]
 */
unsigned short CPUZ80::build16BitAddress() {
    unsigned char addrLo = NB();
    unsigned char addrHi = NB();

    return (addrLo + (addrHi << 8));
}

/**
 * [CPUZ80::get16BitRelativeValue Returns the value stored within the given memory address]
 * @return [description]
 */
unsigned char CPUZ80::getIndirectValue() {
    return memory->read(build16BitAddress());
}

/**
 * [CPUZ80::get16BitRelativeValue Returns the value stored within the given memory address]
 * @return [description]
 */
unsigned char CPUZ80::getIndirectValue(unsigned short address) {
    return memory->read(address);
}

/**
 * [CPUZ80::handleSignFlag Set the sign flag if value could be interpreted as negative (If the most significant bit of the value is set)]
 * @param value [The value to operate on]
 */
void CPUZ80::handleSignFlag(unsigned char value) {
    setFlag(CPUFlag::sign, value > 0x7F);
}

/**
 * [CPUZ80::handleZeroFlag Set the zero flag if the value is zero]
 * @param value [The value to operate on]
 */
void CPUZ80::handleZeroFlag(unsigned char value) {
    setFlag(CPUFlag::zero, value == 0x0);
}

/**
 * [CPUZ80::handleOverflowFlag Sets the overflow flag if the given number can't fit in the register]
 * @param value [The value to operate on]
 */
void CPUZ80::handleOverflowFlag(unsigned short value) {
    // TODO: Handle this properly
    setFlag(CPUFlag::overflow, value > 0xFF);
}

/**
 * [CPUZ80::handleCarryFlag Sets the overflow flag if the given number can't fit in the register]
 * @param value [The value to operate on]
 */
void CPUZ80::handleCarryFlag(unsigned short value) {
    setFlag(CPUFlag::carry, value > 0xFF);
}

/**
 * [CPUZ80::handleCarryFlag Sets the overflow flag if the given number can't fit in the register]
 * @param value [The value to operate on]
 */
void CPUZ80::handleHalfCarryFlag(unsigned char value) {
    setFlag(CPUFlag::halfCarry, value > 0xF);
}
