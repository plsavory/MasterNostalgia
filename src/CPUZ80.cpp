/*
Mastalgia - Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#define VERBOSE_MODE

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

    cyclesTaken = 0;

    resetRequest = ResetRequest::none;

    // Reset the CPU to its initial state
    reset();
}

CPUZ80::~CPUZ80() {

}

void CPUZ80::reset() {

    resetRequest = ResetRequest::none;
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

    if (resetRequest == ResetRequest::pending) {
        resetRequest = ResetRequest::processing;
        pushStack(programCounter);
        iff1 = false;
        state = CPUState::Running;
        programCounter = 0x66;
    }

    if (z80Io->isVDPRequestingInterrupt() && iff1 && interruptMode == 1) {
        state = CPUState::Running;
        pushStack(programCounter);
        programCounter = 0x38;
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

    switch (opcode) {
        case 0x00:
            // nop
            cyclesTaken = 4;
            break;
        case 0x01:
            // ld bc, nn
            ldReg16(gpRegisters[cpuReg::BC].whole, build16BitNumber(), false);
            cyclesTaken = 10;
            break;
        case 0x02:
            // ld (bc), a
            writeMemory(gpRegisters[cpuReg::BC].whole, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 7;
            break;
        case 0x03:
            // inc bc
            inc16Bit(gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 6;
            break;
        case 0x04:
            // inc b
            inc8Bit(gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x05:
            // dec b
            dec8Bit(gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x06:
            // ld b, n
            ldReg8(gpRegisters[cpuReg::BC].hi, NB());
            cyclesTaken = 7;
            break;
        case 0x07:
            // rlca
            rlca();
            cyclesTaken = 4;
            break;
        case 0x08:
            // ex af, af'
            exchange16Bit(gpRegisters[cpuReg::AF].whole, gpRegisters[cpuReg::AFS].whole);
            cyclesTaken = 4;
            break;
        case 0x09:
            // add hl, bc
            add16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 11;
            break;
        case 0x0A:
            // ld a, (bc)
            ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::BC].whole));
            cyclesTaken = 7;
            break;
        case 0x0B:
            // dec bc
            dec16Bit(gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 6;
            break;
        case 0x0C:
            // inc c
            inc8Bit(gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x0D:
            // dec c
            dec8Bit(gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x0E:
            // ld c, n
            ldReg8(gpRegisters[cpuReg::BC].lo, NB());
            cyclesTaken = 7;
            break;
        case 0x0F:
            // rrca
            rrca();
            cyclesTaken = 4;
            break;
        case 0x10:
            // djnz, d
            djnz();
            break;
        case 0x11:
            // ld de, nn
            ldReg16(gpRegisters[cpuReg::DE].whole, build16BitNumber(), false);
            cyclesTaken = 10;
            break;
        case 0x12:
            // ld (de), a
            writeMemory(gpRegisters[cpuReg::DE].whole, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 7;
            break;
        case 0x13:
            // inc de
            inc16Bit(gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 6;
            break;
        case 0x14:
            // inc d
            inc8Bit(gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x15:
            // dec d
            dec8Bit(gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x16:
            // ld d, n
            ldReg8(gpRegisters[cpuReg::DE].hi, NB());
            cyclesTaken = 7;
            break;
        case 0x17:
            // rla
            rla();
            cyclesTaken = 4;
            break;
        case 0x18:
            // jr d
            jr();
            break;
        case 0x19:
            // add hl, de
            add16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 11;
            break;
        case 0X1A:
            // ld a, (de)
            ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::DE].whole));
            cyclesTaken = 7;
            break;
        case 0x1B:
            // dec de
            dec16Bit(gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 6;
            break;
        case 0x1C:
            // inc e
            inc8Bit(gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x1D:
            // dec e
            dec8Bit(gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x1E:
            // ld e, n
            ldReg8(gpRegisters[cpuReg::DE].lo, NB());
            cyclesTaken = 7;
            break;
        case 0x1F:
            // rra
            rra();
            cyclesTaken = 4;
            break;
        case 0x20:
            // jr nz, d
            jrCondition(JPCondition::NZ);
            break;
        case 0x21:
            // ld hl, nn
            ldReg16(gpRegisters[cpuReg::HL].whole, build16BitNumber(), false);
            cyclesTaken = 10;
            break;
        case 0x22:
            // ld (nn), hl
            store(build16BitNumber(), gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 16;
            break;
        case 0x23:
            // inc hl
            inc16Bit(gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 6;
            break;
        case 0x24:
            // inc h
            inc8Bit(gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x25:
            // dec h
            dec8Bit(gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x26:
            // ld h, n
            ldReg8(gpRegisters[cpuReg::HL].hi, NB());
            cyclesTaken = 7;
            break;
        case 0x27:
            // daa
            da(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x28:
            // jr z, d
            jrCondition(JPCondition::Z);
            break;
        case 0x29:
            // add hl, hl
            add16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 11;
            break;
        case 0x2A:
            // ld hl, (nn)
            ldReg16(gpRegisters[cpuReg::HL].whole, readMemory16Bit(build16BitNumber()), false);
            cyclesTaken = 16;
            break;
        case 0x2B:
            // dec hl
            dec16Bit(gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 6;
            break;
        case 0x2C:
            // inc l
            inc8Bit(gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x2D:
            // dec l
            dec8Bit(gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x2E:
            // ld l, n
            ldReg8(gpRegisters[cpuReg::HL].lo, NB());
            cyclesTaken = 7;
            break;
        case 0x2F:
            // cpl
            cpl(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x30:
            // jr nc, d
            jrCondition(JPCondition::NC);
            break;
        case 0x31:
            // ld sp, nn
            ldReg16(stackPointer, build16BitNumber(), false);
            cyclesTaken = 10;
            break;
        case 0x32:
            // ld (nn), a
            writeMemory(build16BitNumber(), gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 13;
            break;
        case 0x33:
            // inc sp
            inc16Bit(stackPointer);
            cyclesTaken = 6;
            break;
        case 0x34:
            // inc (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, getInc8BitValue(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 11;
            break;
        case 0x35:
            // dec (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, getDec8BitValue(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 11;
            break;
        case 0x36:
            // ld (hl), n
            writeMemory(gpRegisters[cpuReg::HL].whole, NB());
            cyclesTaken = 10;
            break;
        case 0x37:
            // scf
            setFlag(CPUFlag::carry, true);
            setFlag(CPUFlag::subtractNegative, false);
            setFlag(CPUFlag::halfCarry, false);
            cyclesTaken = 4;
            break;
        case 0x38:
            // jr c, d
            jrCondition(JPCondition::C);
            break;
        case 0x39:
            // add hl, sp
            add16Bit(gpRegisters[cpuReg::HL].whole, stackPointer);
            cyclesTaken = 11;
            break;
        case 0x3A:
            // ld a, (nn)
            ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(build16BitNumber()));
            cyclesTaken = 13;
            break;
        case 0x3B:
            // dec sp
            dec16Bit(stackPointer);
            cyclesTaken = 6;
            break;
        case 0x3C:
            // inc a
            inc8Bit(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
             break;
        case 0x3D:
            // dec a
            dec8Bit(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x3E:
            // ld a, n
            ldReg8(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0x3F:
            // ccf
            ccf();
            cyclesTaken = 4;
            break;
        case 0x40:
            // ld b, b (Why does this even exist?)
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x41:
            // ld b, c
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x42:
            // ld b, d
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x43:
            // ld b, e
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x44:
            // ld b, h
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x45:
            // ld b, l
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x46:
            // ld b, (hl)
            ldReg8(gpRegisters[cpuReg::BC].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x47:
            // ld b, a
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x48:
            // ld c, b
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x49:
            // ld c, c
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x4A:
            // ld c, d
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x4B:
            // ld c, e
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x4C:
            // ld c, h
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x4D:
            // ld c, l
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x4E:
            // ld c, (hl)
            ldReg8(gpRegisters[cpuReg::BC].lo, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x4F:
            // ld c, a
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x50:
            // ld d, b
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x51:
            // ld d, c
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x52:
            // ld d, d
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x53:
            // ld d, e
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x54:
            // ld d, h
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x55:
            // ld d, l
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x56:
            // ld d, (hl)
            ldReg8(gpRegisters[cpuReg::DE].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x57:
            // ld d, a
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x58:
            // ld e, b
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[BC].hi);
            cyclesTaken = 4;
            break;
        case 0x59:
            // ld e, c
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x5A:
            // ld e, d
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x5B:
            // ld e, e
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x5C:
            // ld e, h
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x5D:
            // ld e, l
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x5E:
            // ld e, (hl)
            ldReg8(gpRegisters[cpuReg::DE].lo, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x5F:
            // ld e, a
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x60:
            // ld h, b
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x61:
            // ld h, c
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x62:
            // ld h, d
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x63:
            // ld h, e
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x64:
            // ld h, h
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x65:
            // ld h, l
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x66:
            // ld h, (hl)
            ldReg8(gpRegisters[cpuReg::HL].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x67:
            // ld h, a
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x68:
            // ld l, b
            ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x69:
            // ld l, c
            ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x6A:
            // ld l, d
            ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x6B:
            // ld l, e
            ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x6C:
            // ld l, h
            ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x6D:
            // ld l, l
            ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x6E:
            // ld l, (hl)
            ldReg8(gpRegisters[cpuReg::HL].lo, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x6F:
            // ld l, a
            ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x70:
            // ld (hl), b
            writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 7;
            break;
        case 0x71:
            // ld (hl), c
            writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 7;
            break;
        case 0x72:
            // ld (hl), d
            writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 7;
            break;
        case 0x73:
            // ld (hl), e
            writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 7;
            break;
        case 0x74:
            // ld (hl), h
            writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 7;
            break;
        case 0x75:
            // ld (hl), l
            writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 7;
            break;
        case 0x76:
            // halt
            state = CPUState::Halt;
            cyclesTaken = 4;
            break;
        case 0x77:
            // ld (hl), a
            writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 7;
            break;
        case 0x78:
            // ld a, b
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x79:
            // ld a, c
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x7A:
            // ld a, d
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x7B:
            // ld a, e
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x7C:
            // ld a, h
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x7D:
            // ld a, l
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x7E:
            // ld a (hl)
            ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x7F:
            // ld a, a
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x80:
            // add a, b
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x81:
            // add a, c
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x82:
            // add a, d
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x83:
            // add a, e
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x84:
            // add a, h
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x85:
            // add a, l
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x86:
            // add a, (hl)
            add8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x87:
            // add a, a
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x88:
            // adc a, b
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x89:
            // adc a, c
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x8A:
            // adc a, d
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x8B:
            // adc a, e
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x8C:
            // adc a, h
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x8D:
            // adc a, l
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x8E:
            // adc a, (hl)
            adc8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x8F:
            // adc a, a
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x90:
            // sub b
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x91:
            // sub c
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x92:
            // sub d
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x93:
            // sub e
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x94:
            // sub h
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x95:
            // sub l
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x96:
            // sub (hl)
            sub8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x97:
            // sub a
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x98:
            // sbc a, b
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0x99:
            // sbc a, c
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0x9A:
            // sbc a, d
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0x9B:
            // sbc a, e
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x9C:
            // sbc a, h
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0x9D:
            // sbc a, l
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0x9E:
            // sbc a, (hl)
            sbc8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x9F:
            // sbc a, a
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0xA0:
            // and b
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0xA1:
            // and c
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0xA2:
            // and d
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0xA3:
            // and e
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0xA4:
            // and h
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0xA5:
            // and l
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0xA6:
            // and (hl)
            and8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 4;
            break;
        case 0xA7:
            // and a
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0xA8:
            // xor b
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0xA9:
            // xor c
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0xAA:
            // xor d
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0xAB:
            // xor e
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0xAC:
            // xor h
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0xAD:
            // xor l
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0xAE:
            // xor (hl)
            exclusiveOr(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0xAF:
            // xor a
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0xB0:
            // or b
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0xB1:
            // or c
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0xB2:
            // or d
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0xB3:
            // or e
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0xB4:
            // or h
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0xB5:
            // or l
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0xB6:
            // or (hl)
            or8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0xB7:
            // or a
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0xB8:
            // cp b
            compare8Bit(gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 4;
            break;
        case 0xB9:
            // cp c
            compare8Bit(gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0xBA:
            // cp d
            compare8Bit(gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 4;
            break;
        case 0xBB:
            // cp e
            compare8Bit(gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0xBC:
            // cp h
            compare8Bit(gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 4;
            break;
        case 0xBD:
            // cp l
            compare8Bit(gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 4;
            break;
        case 0xBE:
            // cp (hl)
            compare8Bit(readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0xBF:
            // cp a
            compare8Bit(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0xC0:
            // ret nz
            retCondition(JPCondition::NZ);
            break;
        case 0xC1:
            //pop bc
            gpRegisters[cpuReg::BC].whole = popStack16();
            cyclesTaken = 10;
            break;
        case 0xC2:
            jpCondition(JPCondition::NZ);
            break;
        case 0xC3:
            // jp nn
            jpImm();
            cyclesTaken = 10;
            break;
        case 0xC4:
            // call nz, nn
            callCondition(JPCondition::NZ);
            break;
        case 0xC5:
            // push bc
            pushStack(gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 11;
            break;
        case 0xC6:
            // add a, n
            add8Bit(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0xC7:
            // rst 00h
            rst(0x00);
            break;
        case 0xC8:
            // ret z
            retCondition(JPCondition::Z);
            break;
        case 0xC9:
            // ret
            programCounter = popStack16();
            cyclesTaken = 10;
            break;
        case 0xCA:
            // jp z, nn
            jpCondition(JPCondition::Z);
            break;
        case 0xCB:
            // bit opcodes
            bitOpcodes();
            break;
        case 0xCC:
            // call z, nn
            callCondition(JPCondition::Z);
            break;
        case 0xCD:
            // call nn
            call(build16BitNumber());
            break;
        case 0xCE:
            // adc a, n
            adc8Bit(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0xCF:
            // rst 08h
            rst(0x08);
            break;
        case 0xD0:
            // ret nc
            retCondition(JPCondition::NC);
            break;
        case 0xD1:
            // pop de
            gpRegisters[cpuReg::DE].whole = popStack16();
            cyclesTaken = 10;
            break;
        case 0xD2:
            // jp nc, nn
            jpCondition(JPCondition::NC);
            break;
        case 0xD3:
            // out (n) a
            portOut(NB(), gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 11;
            break;
        case 0xD4:
            // call nc, nn
            call(build16BitNumber(), !getFlag(CPUFlag::carry));
            break;
        case 0xD5:
            // push de
            pushStack(gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 11;
            break;
        case 0xD6:
            // sub n
            sub8Bit(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0xD7:
            // rst 10h
            rst(0x10);
            break;
        case 0xD8:
            // ret c
            retCondition(JPCondition::C);
            break;
        case 0xD9:
            // exx
            exchange16Bit(gpRegisters[cpuReg::BC].whole, gpRegisters[cpuReg::BCS].whole);
            exchange16Bit(gpRegisters[cpuReg::DE].whole, gpRegisters[cpuReg::DES].whole);
            exchange16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HLS].whole);
            cyclesTaken = 4;
            break;
        case 0xDA:
            // jp c, nn
            jpCondition(JPCondition::C);
            break;
        case 0xDB:
            // in a, (n)
            readPortToRegister(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 11;
            break;
        case 0xDC:
            // call c, nn
            callCondition(JPCondition::C);
            break;
        case 0xDD:
            // ix opcodes
            ixOpcodes();
            break;
        case 0xDE:
            // sbc a, n
            sbc8Bit(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0xDF:
            // rst 18h
            rst(0x18);
            break;
        case 0xE0:
            // ret po
            retCondition(JPCondition::PO);
            break;
        case 0xE1:
            // pop hl
            gpRegisters[cpuReg::HL].whole = popStack16();
            cyclesTaken = 10;
            break;
        case 0xE2:
            // jp po, nn
            jpCondition(JPCondition::PO);
            break;
        case 0xE3:
            // ex (sp), hl
            exStack(gpRegisters[cpuReg::HL].whole);
            break;
        case 0xE4:
            // call po, nn
            call(build16BitNumber(), !getFlag(CPUFlag::overflowParity));
            break;
        case 0xE5:
            // push hl
            pushStack(gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 11;
            break;
        case 0xE6:
            // and n
            and8Bit(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0xE7:
            // rst 20h
            rst(0x20);
            break;
        case 0xE8:
            // ret pe
            retCondition(JPCondition::PE);
            break;
        case 0xE9:
            // jp (hl) - This is not indirect, though the notation implies so
            programCounter = gpRegisters[cpuReg::HL].whole;
            cyclesTaken = 4;
            break;
        case 0xEA:
            // jp pe, nn
            jpCondition(JPCondition::PE);
            break;
        case 0xEB:
            // ex de, hl
            exchange16Bit(gpRegisters[cpuReg::DE].whole, gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 4;
            break;
        case 0xEC:
            // call pe, nn
            callCondition(JPCondition::PE);
            break;
        case 0xED:
            extendedOpcodes();
            break;
        case 0xEE:
            // xor n
            exclusiveOr(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0xEF:
            // rst 28h
            rst(0x28);
            break;
        case 0xF0:
            // ret p
            retCondition(JPCondition::P);
            break;
        case 0xF1:
            // pop af
            gpRegisters[cpuReg::AF].whole = popStack16();
            cyclesTaken = 10;
            break;
        case 0xF2:
            // jp p, nn
            jpCondition(JPCondition::P);
            cyclesTaken  = 10;
            break;
        case 0xF3:
            // di
            iff1 = iff2 = false;
            cyclesTaken = 4;
            break;
        case 0xF4:
            // call p, nn
            callCondition(JPCondition::P);
            break;
        case 0xF5:
            // push af
            pushStack(gpRegisters[cpuReg::AF].whole);
            cyclesTaken = 11;
            break;
        case 0xF6:
            // or n
            or8Bit(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 7;
            break;
        case 0xF7:
            // rst 30h
            rst(0x30);
            break;
        case 0xF8:
            // ret m
            retCondition(JPCondition::M);
            break;
        case 0xF9:
            // ld sp, hl
            ldReg16(stackPointer, gpRegisters[cpuReg::HL].whole, false);
            cyclesTaken = 6;
            break;
        case 0xFA:
            // jp m, nn
            jpCondition(JPCondition::M);
            break;
        case 0xFB:
            // ei
            enableInterrupts = true; // Interrupts should be re-enabled when executing the next instruction
            cyclesTaken = 4;
            break;
        case 0xFC:
            // call m, nn
            callCondition(JPCondition::M);
            break;
        case 0xFD:
            iyOpcodes();
            break;
        case 0xFE:
            // cp n
            compare8Bit(NB());
            cyclesTaken = 7;
            break;
        case 0xFF:
            // rst 38h
            rst(0x38);
            break;
        default:
            std::stringstream ss;
            ss << "Unimplemented opcode: 0x" << std::hex << (int) opcode << "-  At PC: 0x"
                      << (int) originalProgramCounterValue << std::endl;
            throw Z80Exception(ss.str());
    }

    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(opcode, 0x0, 0x0);
    }

#ifdef VERBOSE_MODE
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

    switch (opcode) {
        case 0x40:
            // in b, (c)
            readPortToRegister(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].lo);
            break;
        case 0x41:
            // out (c), b
            portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 12;
            break;
        case 0x42:
            // sbc hl, bc
            sbc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 15;
            break;
        case 0x43:
            // ld (nn), bc
            writeMemory(build16BitNumber(), gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 20;
            break;
        case 0x44:
            // neg
            neg();
            cyclesTaken = 8;
            break;
        case 0x45:
            // retn
            retn();
            break;
        case 0x46:
            // im 0
            setInterruptMode(0);
            break;
        case 0x47:
            // ld i, a
            ldReg8(registerI, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 9;
            break;
        case 0x48:
            // in c, (c)
            readPortToRegister(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].lo);
            break;
        case 0x49:
            // out (c), c
            portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 12;
            break;
        case 0x4A:
            // adc hl, bc
            adc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 15;
            break;
        case 0x4B:
            // ld bc, (nn)
            ldReg16(gpRegisters[cpuReg::BC].whole, readMemory16Bit(build16BitNumber()), false);
            cyclesTaken = 20;
            break;
        case 0x4D:
            // reti
            reti();
            break;
        case 0x4F:
            // ld r, a
            ldReg8(registerR, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 9;
            break;
        case 0x50:
            // in d, (c)
            readPortToRegister(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].lo);
            break;
        case 0x51:
            // out (c), d
            portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 12;
            break;
        case 0x52:
            // sbc hl, de
            sbc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 15;
            break;
        case 0x53:
            // ld (nn), de
            writeMemory(build16BitNumber(), gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 20;
            break;
        case 0x56:
            // im 1
            setInterruptMode(1);
            break;
        case 0x57:
            // ld a, i
            ldReg8(gpRegisters[cpuReg::AF].hi, registerI, true);
            cyclesTaken = 9;
            break;
        case 0x58:
            // in e, (c)
            readPortToRegister(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 12;
            break;
        case 0x59:
            // out (c), e
            portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 12;
            break;
        case 0x5A:
            // adc hl, de
            adc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 15;
            break;
        case 0x5B:
            // ld de, (nn)
            ldReg16(gpRegisters[cpuReg::DE].whole, readMemory16Bit(build16BitNumber()));
            cyclesTaken = 20;
            break;
        case 0x5E:
            setInterruptMode(2);
            break;
        case 0x5F:
            // ld a, r
            ldReg8(gpRegisters[cpuReg::AF].hi, registerR, true);
            cyclesTaken = 9;
            break;
        case 0x60:
            // in h, (c)
            readPortToRegister(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 12;
            break;
        case 0x61:
            // out (c), h
            portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 12;
            break;
        case 0x62:
            // sbc hl, hl
            sbc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 15;
            break;
        case 0x63:
            // ld (nn), hl
            writeMemory(build16BitNumber(), gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 20;
            break;
        case 0x67:
            // rrd
            rrd(gpRegisters[cpuReg::AF].hi);
            break;
        case 0x68:
            // in l, (c)
            readPortToRegister(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 12;
            break;
        case 0x69:
            // out (c), l
            portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 12;
            break;
        case 0x6A:
            // adc hl, hl
            adc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 15;
            break;
        case 0x6B:
            // ld hl, (nn)
            ldReg16(gpRegisters[cpuReg::HL].whole, readMemory16Bit(build16BitNumber()));
            cyclesTaken = 20;
            break;
        case 0x6F:
            // rld
            rld(gpRegisters[cpuReg::AF].hi);
            break;
        case 0x70:
            // in (c)
            z80Io->read(gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 12;
            break;
        case 0x71:
            // out (c), 0
            portOut(gpRegisters[cpuReg::BC].lo, 0);
            cyclesTaken = 12;
            break;
        case 0x72:
            // sbc hl, sp
            sbc16Bit(gpRegisters[cpuReg::HL].whole, stackPointer);
            cyclesTaken = 15;
            break;
        case 0x73:
            // ld (nn), sp
            writeMemory(build16BitNumber(), stackPointer);
            cyclesTaken = 20;
            break;
        case 0x78:
            // in a, (c)
            readPortToRegister(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 12;
            break;
        case 0x79:
            // out (c), a
            portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 12;
            break;
        case 0x7A:
            // adc hl, sp
            adc16Bit(gpRegisters[cpuReg::HL].whole, stackPointer);
            cyclesTaken = 15;
            break;
        case 0x7B:
            // ld sp, (nn)
            ldReg16(stackPointer, readMemory16Bit(build16BitNumber()));
            cyclesTaken = 20;
            break;
        case 0xA0:
            // ldi
            ldi(true);
            break;
        case 0xA1:
            // cpi
            cpi(true);
            break;
        case 0xA2:
            // ini
            ini(true);
            break;
        case 0xA3:
            // outi
            outi(true);
            break;
        case 0xA8:
            // ldd
            ldi(false);
            break;
        case 0xA9:
            // cpd
            cpi(false);
            break;
        case 0xAA:
            ini(false);
            break;
        case 0xAB:
            outi(false);
            break;
        case 0xB0:
            // ldir
            ldir(true);
            break;
        case 0xB1:
            // cpir
            cpir(true);
            break;
        case 0xB2:
            // inir
            inir(true);
            break;
        case 0xB3:
            // otir
            otir(true);
            break;
        case 0xB8:
            // lddr
            ldir(false);
            break;
        case 0xB9:
            // cpdr
            cpir(false);
            break;
        case 0xBA:
            // indr
            inir(false);
            break;
        case 0xBB:
            // otdr
            otir(false);
            break;
        default:
            std::stringstream ss;
            ss << "Unimplemented extended opcode: 0x" << std::hex << (int) opcode;
            throw Z80Exception(ss.str());
    }

    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(0xED, opcode, 0x0);
    }
}

void CPUZ80::ixOpcodes() {
    indexOpcodes(0xDD, "IX", cpuReg::IX);
}

void CPUZ80::indexOpcodes(unsigned char opcodePrefix, const std::string& indexPrefix, cpuReg indexRegister) {

    // TODO add the rest of the undocumented opcodes, not all of them are done.

    unsigned char opcode = NBHideFromTrace();
    displayOpcodePrefix = opcodePrefix;
    displayOpcode = opcode;
    unsigned short address = 0x0;

    switch (opcode) {
        case 0x04:
            // inc b
            inc8Bit(gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x05:
            // dec b
            dec8Bit(gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x06:
            // ld b, n
            ldReg8(gpRegisters[cpuReg::BC].hi, NB());
            cyclesTaken = 11;
            break;
        case 0x09:
            // add ix, bc
            add16Bit(gpRegisters[indexRegister].whole, gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 15;
            break;
        case 0x0C:
            // inc c
            inc8Bit(gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x0D:
            // dec c
            dec8Bit(gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x0E:
            // ld c, n
            ldReg8(gpRegisters[cpuReg::BC].lo, NB());
            cyclesTaken = 11;
            break;
        case 0x14:
            // inc d
            inc8Bit(gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x15:
            // dec d
            dec8Bit(gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x16:
            // ld d, n
            ldReg8(gpRegisters[cpuReg::DE].hi, NB());
            cyclesTaken = 11;
            break;
        case 0x19:
            // add ix,de
            add16Bit(gpRegisters[indexRegister].whole, gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 15;
            break;
        case 0x1C:
            // inc e
            inc8Bit(gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x1D:
            // dec e
            dec8Bit(gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x1E:
            // ld e, n
            ldReg8(gpRegisters[cpuReg::DE].lo, NB());
            cyclesTaken = 11;
            break;
        case 0x21:
            // ld ix, nn
            ldReg16(gpRegisters[indexRegister].whole, build16BitNumber());
            cyclesTaken = 14;
            break;
        case 0x22:
            // ld (nn), ix
            writeMemory(build16BitNumber(), gpRegisters[indexRegister].whole);
            cyclesTaken = 20;
            break;
        case 0x23:
            // inc ix
            inc16Bit(gpRegisters[indexRegister].whole);
            cyclesTaken = 10;
            break;
        case 0x24:
            // inc ixh
            inc8Bit(gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x25:
            // dec ixh
            dec8Bit(gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x26:
            // ld ixh, n
            ldReg8(gpRegisters[indexRegister].hi, NB());
            cyclesTaken = 11;
            break;
        case 0x29:
            // add ix, sp
            add16Bit(gpRegisters[indexRegister].whole, gpRegisters[indexRegister].whole);
            cyclesTaken = 15;
            break;
        case 0x2A:
            // ld ix, (nn)
            ldReg16(gpRegisters[indexRegister].whole, readMemory16Bit(build16BitNumber()));
            cyclesTaken = 20;
            break;
        case 0x2B:
            // dec ix
            dec16Bit(gpRegisters[indexRegister].whole);
            cyclesTaken = 10;
            break;
        case 0x2C:
            // inc ixl
            inc8Bit(gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x2D:
            // dec ixl
            dec8Bit(gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x2E:
            // ld ixl, n
            ldReg8(gpRegisters[indexRegister].lo, NB());
            cyclesTaken = 11;
            break;
        case 0x34:
            // inc (ix+d)
            address = getIndexedOffsetAddress(gpRegisters[indexRegister].whole);
            writeMemory(address, getInc8BitValue(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x35:
            // dec (ix+d)
            address = getIndexedOffsetAddress(gpRegisters[indexRegister].whole);
            writeMemory(address, getDec8BitValue(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x36:
            // ld (ix+d), n
            writeMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole), NB());
            cyclesTaken = 19;
            break;
        case 0x39:
            // add ix, sp
            add16Bit(gpRegisters[indexRegister].whole, stackPointer);
            cyclesTaken = 15;
            break;
        case 0x3C:
            // inc a
            inc8Bit(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x3D:
            // dec a
            dec8Bit(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x3E:
            // ld a, n
            ldReg8(gpRegisters[cpuReg::AF].hi, NB());
            cyclesTaken = 11;
            break;
        case 0x40:
            // ld b, b
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x41:
            // ld b, c
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x42:
            // ld b, d
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x43:
            // ld b, e
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x44:
            // ld b, ixh
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x45:
            // ld b, ixl
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x46:
            // ld b, (ix+d)
            ldReg8(gpRegisters[cpuReg::BC].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x47:
            // ld b, a
            ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x48:
            // ld c, b
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x49:
            // ld c, c
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x4A:
            // ld c, d
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x4B:
            // ld c, e
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x4C:
            // ld c, ixh
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x4D:
            // ld c, ixl
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x4E:
            // ld c, (ix+d)
            ldReg8(gpRegisters[cpuReg::BC].lo, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x4F:
            // ld c, a
            ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x50:
            // ld d, b
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x51:
            // ld d, c
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x52:
            // ld d, d
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x53:
            // ld d, e
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x54:
            // ld d, ixh
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x55:
            // ld d, ixl
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x56:
            // ld d, (ix+d)
            ldReg8(gpRegisters[cpuReg::DE].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x57:
            // ld d, a
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x58:
            // ld e, b
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x59:
            // ld e, c
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x5A:
            // ld e, d
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x5B:
            // ld e, e
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x5C:
            // ld e, ixh
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x5D:
            // ld e, ixl
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x5E:
            // ld e, (ix+d)
            ldReg8(gpRegisters[cpuReg::DE].lo, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x5F:
            // ld e, a
            ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x60:
            // ld ixh, b
            ldReg8(gpRegisters[indexRegister].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x61:
            // ld ixh, c
            ldReg8(gpRegisters[indexRegister].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x62:
            // ld ixh, d
            ldReg8(gpRegisters[indexRegister].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x63:
            // ld ixh, e
            ldReg8(gpRegisters[indexRegister].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x64:
            // ld ixh, ixh
            ldReg8(gpRegisters[indexRegister].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x65:
            // ld ixh, ixl
            ldReg8(gpRegisters[indexRegister].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x66:
            // ld h, (ix+d)
            ldReg8(gpRegisters[cpuReg::HL].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x67:
            // ld ixh, a
            ldReg8(gpRegisters[indexRegister].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x68:
            // ld ixl, b
            ldReg8(gpRegisters[indexRegister].lo, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x69:
            // ld ixl, c
            ldReg8(gpRegisters[indexRegister].lo, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x6A:
            // ld ixl, d
            ldReg8(gpRegisters[indexRegister].lo, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x6B:
            // ld ixl, e
            ldReg8(gpRegisters[indexRegister].lo, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x6C:
            // ld ixl, ixh
            ldReg8(gpRegisters[indexRegister].lo, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x6D:
            // ld ixl, ixl
            ldReg8(gpRegisters[indexRegister].lo, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x6E:
            // ld l, (ix+d)
            ldReg8(gpRegisters[cpuReg::HL].lo, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x6F:
            // ld ixl, a
            ldReg8(gpRegisters[indexRegister].lo, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x70:
            // ld (ix+d), b
            writeMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole), gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 19;
            break;
        case 0x71:
            // ld (ix+d), c
            writeMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole), gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 19;
            break;
        case 0x72:
            // ld (ix+d), d
            writeMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole), gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 19;
            break;
        case 0x73:
            // ld (ix+d), e
            writeMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole), gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 19;
            break;
        case 0x74:
            // ld (ix+d), h
            writeMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole), gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 19;
            break;
        case 0x75:
            // ld (ix+d), l
            writeMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole), gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 19;
            break;
        case 0x77:
            // ld (ix+d), a
            writeMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole), gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 19;
            break;
        case 0x78:
            // ld a, b
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x79:
            // ld a, c
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x7A:
            // ld a, d
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x7B:
            // ld a, e
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x7C:
            // ld a, ixh
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x7D:
            // ld a, ixl
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x7E:
            // ld a, (ix+d)
            ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x7F:
            // ld a, a
            ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x80:
            // add a, b
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x81:
            // add a, c
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x82:
            // add a, d
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x83:
            // add a, e
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x84:
            // add a, ixh
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x85:
            // add a, ixl
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x86:
            // add a, (ix+d)
            add8Bit(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x87:
            // add a, a
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x88:
            // adc a, b
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x89:
            // adc a, c
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x8A:
            // adc a, d
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x8B:
            // adc a, e
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x8C:
            // adc a, ixh
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x8D:
            // adc a, ixl
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x8E:
            // adc a, (ix+d)
            adc8Bit(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x8F:
            // adc a, a
            adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x90:
            // sub a, b
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x91:
            // sub a, c
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x92:
            // sub a, d
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x93:
            // sub a, e
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x94:
            // sub a, ixh
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x95:
            // sub a, ixl
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x96:
            // sub a, (ix+d)
            sub8Bit(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x97:
            // sub a, a
            sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x98:
            // sbc a, b
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x99:
            // sbc a, c
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x9A:
            // sbc a, d
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x9B:
            // sbc a, e
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x9C:
            // sbc a, ixh
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0x9D:
            // sbc a, ixl
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0x9E:
            // sbc a, (ix+d)
            sbc8Bit(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0x9F:
            // sbc a, a
            sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0xA0:
            // and b
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0xA1:
            // and c
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0xA2:
            // and d
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0xA3:
            // and e
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0xA4:
            // and ixh
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0xA5:
            // and ixl
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0xA6:
            // and a, (ix+d)
            and8Bit(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0xA7:
            // and a
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0xA8:
            // xor b
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0xA9:
            // xor c
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0xAA:
            // xor d
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0xAB:
            // xor e
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0xAC:
            // xor ixh
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0xAD:
            // xor ixl
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0xAE:
            // xor a, (ix+d)
            exclusiveOr(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0xAF:
            // xor a
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0xB0:
            // or b
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0xB1:
            // or c
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0xB2:
            // or d
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0xB3:
            // or e
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0xB4:
            // or ixh
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0xB5:
            // or ixl
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0xB6:
            // or a, (ix+d)
            or8Bit(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0xB7:
            // or a
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0xB8:
            // cp b
            compare8Bit(gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0xB9:
            // cp c
            compare8Bit(gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0xBA:
            // cp d
            compare8Bit(gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0xBB:
            // cp e
            compare8Bit(gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0xBC:
            // cp ixh
            compare8Bit(gpRegisters[indexRegister].hi);
            cyclesTaken = 8;
            break;
        case 0xBD:
            // cp ixl
            compare8Bit(gpRegisters[indexRegister].lo);
            cyclesTaken = 8;
            break;
        case 0xBE:
            // cp a, (ix+d)
            compare8Bit(readMemory(getIndexedOffsetAddress(gpRegisters[indexRegister].whole)));
            cyclesTaken = 19;
            break;
        case 0xBF:
            // cp a
            compare8Bit(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0xCB:
            displayOpcodePrefix = 0xCB;
            indexBitOpcodes(opcodePrefix, indexPrefix, indexRegister);
            break;
        case 0xE1:
            // pop ix
            ldReg16(gpRegisters[indexRegister].whole, popStack16(), false);
            cyclesTaken = 14;
            break;
        case 0xE3:
            // ex (sp), ix
            popStackExchange(gpRegisters[indexRegister].whole);
            cyclesTaken = 23;
            break;
        case 0xE5:
            // push ix
            pushStack(gpRegisters[indexRegister].whole);
            cyclesTaken = 15;
            break;
        case 0xE9:
            // jp (ix) - Notation seems to indicate that this is indirect, but docs says that it is
            programCounter = gpRegisters[indexRegister].whole;
            cyclesTaken = 8;
            break;
        case 0xF9:
            ldReg16(stackPointer, gpRegisters[indexRegister].whole);
            cyclesTaken = 10;
            break;
        default:
            std::stringstream ss;
            ss << "Unimplemented ix opcode: 0x" << std::hex << (int) opcode << std::endl;
            throw Z80Exception(ss.str());
    }

    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(indexRegister == cpuReg::IX ? 0xDD : 0xFD, opcode, 0x0);
    }
}

void CPUZ80::iyOpcodes() {
    indexOpcodes(0xFD, "IY", cpuReg::IY);
}

/**
 * [logCPUState Log the CPU's current state to the console]
 */
void CPUZ80::logCPUState() {
    return;
    std::cout << std::uppercase << "BC="
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

// BIT instructions below here - massive jump tables
// TODO re-attempt trying to calculate this rather than have a switch, I did previously try calculating the instructions, destination registers and bit numbers but it seemed slower to execute than just having a switch.

void CPUZ80::bitOpcodes() {

    unsigned char opcode = NBHideFromTrace();
    displayOpcodePrefix = 0xCB;
    displayOpcode = opcode;

    switch (opcode) {
        case 0x00:
            // rlc b
            ldReg8(gpRegisters[cpuReg::BC].hi, rlc(gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x01:
            // rlc c
            ldReg8(gpRegisters[cpuReg::BC].lo, rlc(gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x02:
            // rlc d
            ldReg8(gpRegisters[cpuReg::DE].hi, rlc(gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x03:
            // rlc e
            ldReg8(gpRegisters[cpuReg::DE].lo, rlc(gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x04:
            // rlc h
            ldReg8(gpRegisters[cpuReg::HL].hi, rlc(gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x05:
            // rlc l
            ldReg8(gpRegisters[cpuReg::HL].lo, rlc(gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x06:
            // rlc (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, rlc(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x07:
            // rlc a
            ldReg8(gpRegisters[cpuReg::AF].hi, rlc(gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x08:
            // rrc b
            ldReg8(gpRegisters[cpuReg::BC].hi, rrc(gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x09:
            // rrc c
            ldReg8(gpRegisters[cpuReg::BC].lo, rrc(gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x0A:
            // rrc d
            ldReg8(gpRegisters[cpuReg::DE].hi, rrc(gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x0B:
            // rrc e
            ldReg8(gpRegisters[cpuReg::DE].lo, rrc(gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x0C:
            // rrc h
            ldReg8(gpRegisters[cpuReg::HL].hi, rrc(gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x0D:
            // rrc l
            ldReg8(gpRegisters[cpuReg::HL].lo, rrc(gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x0E:
            // rrc (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, rrc(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x0F:
            // rrc a
            ldReg8(gpRegisters[cpuReg::AF].hi, rrc(gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x10:
            // rl b
            ldReg8(gpRegisters[cpuReg::BC].hi, rl(gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x11:
            // rl c
            ldReg8(gpRegisters[cpuReg::BC].lo, rl(gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x12:
            // rl d
            ldReg8(gpRegisters[cpuReg::DE].hi, rl(gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x13:
            // rl e
            ldReg8(gpRegisters[cpuReg::DE].lo, rl(gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x14:
            // rl h
            ldReg8(gpRegisters[cpuReg::HL].hi, rl(gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x15:
            // rl l
            ldReg8(gpRegisters[cpuReg::HL].lo, rl(gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x16:
            // rl (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, rl(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x17:
            // rl a
            ldReg8(gpRegisters[cpuReg::AF].hi, rl(gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x18:
            // rr b
            ldReg8(gpRegisters[cpuReg::BC].hi, rr(gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x19:
            // rr c
            ldReg8(gpRegisters[cpuReg::BC].lo, rr(gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x1A:
            // rr d
            ldReg8(gpRegisters[cpuReg::DE].hi, rr(gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x1B:
            // rr e
            ldReg8(gpRegisters[cpuReg::DE].lo, rr(gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x1C:
            // rr h
            ldReg8(gpRegisters[cpuReg::HL].hi, rr(gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x1D:
            // rr l
            ldReg8(gpRegisters[cpuReg::HL].lo, rr(gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x1E:
            // rr (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, rr(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x1F:
            // rr a
            ldReg8(gpRegisters[cpuReg::AF].hi, rr(gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x20:
            // sla b
            ldReg8(gpRegisters[cpuReg::BC].hi, sla(gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x21:
            // sla c
            ldReg8(gpRegisters[cpuReg::BC].lo, sla(gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x22:
            // sla d
            ldReg8(gpRegisters[cpuReg::DE].hi, sla(gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x23:
            // sla e
            ldReg8(gpRegisters[cpuReg::DE].lo, sla(gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x24:
            // sla h
            ldReg8(gpRegisters[cpuReg::HL].hi, sla(gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x25:
            // sla l
            ldReg8(gpRegisters[cpuReg::HL].lo, sla(gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x26:
            // sla (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, sla(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x27:
            // sla a
            ldReg8(gpRegisters[cpuReg::AF].hi, sla(gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x28:
            // sra b
            ldReg8(gpRegisters[cpuReg::BC].hi, sra(gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x29:
            // sra c
            ldReg8(gpRegisters[cpuReg::BC].lo, sra(gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x2A:
            // sra d
            ldReg8(gpRegisters[cpuReg::DE].hi, sra(gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x2B:
            // sra e
            ldReg8(gpRegisters[cpuReg::DE].lo, sra(gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x2C:
            // sra h
            ldReg8(gpRegisters[cpuReg::HL].hi, sra(gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x2D:
            // sra l
            ldReg8(gpRegisters[cpuReg::HL].lo, sra(gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x2E:
            // sra (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, sra(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x2F:
            // sra a
            ldReg8(gpRegisters[cpuReg::AF].hi, sra(gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x30:
            // sll b
            ldReg8(gpRegisters[cpuReg::BC].hi, sll(gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x31:
            // sll c
            ldReg8(gpRegisters[cpuReg::BC].lo, sll(gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x32:
            // sll d
            ldReg8(gpRegisters[cpuReg::DE].hi, sll(gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x33:
            // sll e
            ldReg8(gpRegisters[cpuReg::DE].lo, sll(gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x34:
            // sll h
            ldReg8(gpRegisters[cpuReg::HL].hi, sll(gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x35:
            // sll l
            ldReg8(gpRegisters[cpuReg::HL].lo, sll(gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x36:
            // sll (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, sll(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x37:
            // sll a
            ldReg8(gpRegisters[cpuReg::AF].hi, sll(gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x38:
            // srl b
            ldReg8(gpRegisters[cpuReg::BC].hi, srl(gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x39:
            // srl c
            ldReg8(gpRegisters[cpuReg::BC].lo, srl(gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x3A:
            // srl d
            ldReg8(gpRegisters[cpuReg::DE].hi, srl(gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x3B:
            // srl e
            ldReg8(gpRegisters[cpuReg::DE].lo, srl(gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x3C:
            // srl h
            ldReg8(gpRegisters[cpuReg::HL].hi, srl(gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x3D:
            // srl l
            ldReg8(gpRegisters[cpuReg::HL].lo, srl(gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x3E:
            // srl (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, srl(readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x3F:
            // srl a
            ldReg8(gpRegisters[cpuReg::AF].hi, srl(gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x40:
            // bit 0, b
            bit(0, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x41:
            // bit 0, c
            bit(0, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x42:
            // bit 0, d
            bit(0, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x43:
            // bit 0, e
            bit(0, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x44:
            // bit 0, h
            bit(0, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 8;
            break;
        case 0x45:
            // bit 0, l
            bit(0, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 8;
            break;
        case 0x46:
            // bit 0, (hl);
            bit(0, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 12;
            break;
        case 0x47:
            // bit 0, a
            bit(0, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x48:
            // bit 1, b
            bit(1, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x49:
            // bit 1, c
            bit(1, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x4A:
            // bit 1, d
            bit(1, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x4B:
            // bit 1, e
            bit(1, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x4C:
            // bit 1, h
            bit(1, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 8;
            break;
        case 0x4D:
            // bit 1, l
            bit(1, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 8;
            break;
        case 0x4E:
            // bit 1, (hl);
            bit(1, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 12;
            break;
        case 0x4F:
            // bit 1, a
            bit(1, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x50:
            // bit 2, b
            bit(2, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x51:
            // bit 2, c
            bit(2, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x52:
            // bit 2, d
            bit(2, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x53:
            // bit 2, e
            bit(2, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x54:
            // bit 2, h
            bit(2, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 8;
            break;
        case 0x55:
            // bit 2, l
            bit(2, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 8;
            break;
        case 0x56:
            // bit 2, (hl);
            bit(2, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 12;
            break;
        case 0x57:
            // bit 2, a
            bit(2, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x58:
            // bit 3, b
            bit(3, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x59:
            // bit 3, c
            bit(3, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x5A:
            // bit 3, d
            bit(3, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x5B:
            // bit 3, e
            bit(3, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x5C:
            // bit 3, h
            bit(3, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 8;
            break;
        case 0x5D:
            // bit 3, l
            bit(3, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 8;
            break;
        case 0x5E:
            // bit 3, (hl);
            bit(3, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 12;
            break;
        case 0x5F:
            // bit 3, a
            bit(3, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x60:
            // bit 4, b
            bit(4, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x61:
            // bit 4, c
            bit(4, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x62:
            // bit 4, d
            bit(4, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x63:
            // bit 4, e
            bit(4, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x64:
            // bit 4, h
            bit(4, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 8;
            break;
        case 0x65:
            // bit 4, l
            bit(4, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 8;
            break;
        case 0x66:
            // bit 4, (hl);
            bit(4, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 12;
            break;
        case 0x67:
            // bit 4, a
            bit(4, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x68:
            // bit 5, b
            bit(5, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x69:
            // bit 5, c
            bit(5, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x6A:
            // bit 5, d
            bit(5, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x6B:
            // bit 5, e
            bit(5, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x6C:
            // bit 5, h
            bit(5, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 8;
            break;
        case 0x6D:
            // bit 5, l
            bit(5, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 8;
            break;
        case 0x6E:
            // bit 5, (hl);
            bit(5, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 12;
            break;
        case 0x6F:
            // bit 5, a
            bit(5, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x70:
            // bit 6, b
            bit(6, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x71:
            // bit 6, c
            bit(6, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x72:
            // bit 6, d
            bit(6, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x73:
            // bit 6, e
            bit(6, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x74:
            // bit 6, h
            bit(6, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 8;
            break;
        case 0x75:
            // bit 6, l
            bit(6, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 8;
            break;
        case 0x76:
            // bit 6, (hl);
            bit(6, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 12;
            break;
        case 0x77:
            // bit 6, a
            bit(6, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x78:
            // bit 7, b
            bit(7, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 8;
            break;
        case 0x79:
            // bit 7, c
            bit(7, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 8;
            break;
        case 0x7A:
            // bit 7, d
            bit(7, gpRegisters[cpuReg::DE].hi);
            cyclesTaken = 8;
            break;
        case 0x7B:
            // bit 7, e
            bit(7, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 8;
            break;
        case 0x7C:
            // bit 7, h
            bit(7, gpRegisters[cpuReg::HL].hi);
            cyclesTaken = 8;
            break;
        case 0x7D:
            // bit 7, l
            bit(7, gpRegisters[cpuReg::HL].lo);
            cyclesTaken = 8;
            break;
        case 0x7E:
            // bit 7, (hl);
            bit(7, readMemory(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 12;
            break;
        case 0x7F:
            // bit 7, a
            bit(7, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 8;
            break;
        case 0x80:
            // res 0, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(0, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x81:
            // res 0, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(0, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x82:
            // res 0, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(0, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x83:
            // res 0, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(0, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x84:
            // res 0, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(0, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x85:
            // res 0, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(0, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x86:
            // res 0, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, res(0, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x87:
            // res 0, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(0, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x88:
            // res 1, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(1, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x89:
            // res 1, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(1, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x8A:
            // res 1, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(1, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x8B:
            // res 1, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(1, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x8C:
            // res 1, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(1, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x8D:
            // res 1, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(1, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x8E:
            // res 1, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, res(1, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x8F:
            // res 1, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(1, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x90:
            // res 2, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(2, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x91:
            // res 2, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(2, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x92:
            // res 2, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(2, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x93:
            // res 2, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(2, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x94:
            // res 2, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(2, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x95:
            // res 2, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(2, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x96:
            // res 2, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, res(2, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x97:
            // res 2, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(2, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0x98:
            // res 3, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(3, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0x99:
            // res 3, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(3, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0x9A:
            // res 3, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(3, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0x9B:
            // res 3, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(3, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0x9C:
            // res 3, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(3, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0x9D:
            // res 3, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(3, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0x9E:
            // res 3, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, res(3, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0x9F:
            // res 3, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(3, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xA0:
            // res 4, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(4, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xA1:
            // res 4, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(4, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xA2:
            // res 4, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(4, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xA3:
            // res 4, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(4, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xA4:
            // res 4, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(4, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xA5:
            // res 4, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(4, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xA6:
            // res 4, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, res(4, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xA7:
            // res 4, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(4, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xA8:
            // res 5, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(5, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xA9:
            // res 5, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(5, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xAA:
            // res 5, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(5, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xAB:
            // res 5, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(5, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xAC:
            // res 5, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(5, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xAD:
            // res 5, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(5, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xAE:
            // res 5, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, res(5, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xAF:
            // res 5, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(5, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xB0:
            // res 6, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(6, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xB1:
            // res 6, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(6, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xB2:
            // res 6, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(6, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xB3:
            // res 6, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(6, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xB4:
            // res 6, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(6, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xB5:
            // res 6, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(6, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xB6:
            // res 6, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, res(6, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xB7:
            // res 6, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(6, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xB8:
            // res 7, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(7, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xB9:
            // res 7, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(7, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xBA:
            // res 7, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(7, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xBB:
            // res 7, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(7, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xBC:
            // res 7, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(7, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xBD:
            // res 7, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(7, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xBE:
            // res 7, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, res(7, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xBF:
            // res 7, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(7, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xC0:
            // set 0, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(0, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xC1:
            // set 0, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(0, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xC2:
            // set 0, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(0, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xC3:
            // set 0, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(0, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xC4:
            // set 0, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(0, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xC5:
            // set 0, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(0, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xC6:
            // set 0, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, set(0, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xC7:
            // set 0, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(0, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xC8:
            // set 1, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(1, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xC9:
            // set 1, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(1, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xCA:
            // set 1, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(1, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xCB:
            // set 1, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(1, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xCC:
            // set 1, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(1, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xCD:
            // set 1, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(1, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xCE:
            // set 1, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, set(1, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xCF:
            // set 1, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(1, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xD0:
            // set 2, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(2, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xD1:
            // set 2, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(2, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xD2:
            // set 2, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(2, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xD3:
            // set 2, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(2, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xD4:
            // set 2, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(2, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xD5:
            // set 2, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(2, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xD6:
            // set 2, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, set(2, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xD7:
            // set 2, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(2, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xD8:
            // set 3, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(3, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xD9:
            // set 3, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(3, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xDA:
            // set 3, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(3, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xDB:
            // set 3, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(3, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xDC:
            // set 3, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(3, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xDD:
            // set 3, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(3, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xDE:
            // set 3, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, set(3, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xDF:
            // set 3, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(3, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xE0:
            // set 4, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(4, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xE1:
            // set 4, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(4, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xE2:
            // set 4, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(4, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xE3:
            // set 4, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(4, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xE4:
            // set 4, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(4, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xE5:
            // set 4, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(4, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xE6:
            // set 4, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, set(4, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xE7:
            // set 4, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(4, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xE8:
            // set 5, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(5, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xE9:
            // set 5, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(5, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xEA:
            // set 5, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(5, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xEB:
            // set 5, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(5, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xEC:
            // set 5, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(5, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xED:
            // set 5, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(5, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xEE:
            // set 5, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, set(5, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xEF:
            // set 5, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(5, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xF0:
            // set 6, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(6, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xF1:
            // set 6, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(6, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xF2:
            // set 6, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(6, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xF3:
            // set 6, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(6, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xF4:
            // set 6, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(6, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xF5:
            // set 6, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(6, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xF6:
            // set 6, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, set(6, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xF7:
            // set 6, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(6, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        case 0xF8:
            // set 7, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(7, gpRegisters[cpuReg::BC].hi));
            cyclesTaken = 8;
            break;
        case 0xF9:
            // set 7, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(7, gpRegisters[cpuReg::BC].lo));
            cyclesTaken = 8;
            break;
        case 0xFA:
            // set 7, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(7, gpRegisters[cpuReg::DE].hi));
            cyclesTaken = 8;
            break;
        case 0xFB:
            // set 7, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(7, gpRegisters[cpuReg::DE].lo));
            cyclesTaken = 8;
            break;
        case 0xFC:
            // set 7, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(7, gpRegisters[cpuReg::HL].hi));
            cyclesTaken = 8;
            break;
        case 0xFD:
            // set 7, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(7, gpRegisters[cpuReg::HL].lo));
            cyclesTaken = 8;
            break;
        case 0xFE:
            // set 7, (hl)
            writeMemory(gpRegisters[cpuReg::HL].whole, set(7, readMemory(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 15;
            break;
        case 0xFF:
            // set 7, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(7, gpRegisters[cpuReg::AF].hi));
            cyclesTaken = 8;
            break;
        default:
            std::stringstream ss;
            ss << "Unimplemented bit opcode: 0x" << std::hex << (int) opcode << std::endl;
            throw Z80Exception(ss.str());
    }

    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(0xCB, opcode, 0x0);
    }

}

void CPUZ80::indexBitOpcodes(unsigned char opcodePrefix, const std::string& indexPrefix, cpuReg indexRegister) {
    // Opcode format is offset, opcode
    unsigned short address = gpRegisters[indexRegister].whole + signedNB();
    unsigned char opcode = NBHideFromTrace();
    displayOpcodePrefix = (displayOpcodePrefix << 8) + opcodePrefix;
    displayOpcode = opcode;

    switch (opcode) {
        case 0x00:
            // rlc b
            ldReg8(gpRegisters[cpuReg::BC].hi, rlc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x01:
            // rlc c
            ldReg8(gpRegisters[cpuReg::BC].lo, rlc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x02:
            // rlc d
            ldReg8(gpRegisters[cpuReg::DE].hi, rlc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x03:
            // rlc e
            ldReg8(gpRegisters[cpuReg::DE].lo, rlc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x04:
            // rlc h
            ldReg8(gpRegisters[cpuReg::HL].hi, rlc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x05:
            // rlc l
            ldReg8(gpRegisters[cpuReg::HL].lo, rlc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x06:
            // rlc (hl)
            writeMemory(address, rlc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x07:
            // rlc a
            ldReg8(gpRegisters[cpuReg::AF].hi, rlc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x08:
            // rrc b
            ldReg8(gpRegisters[cpuReg::BC].hi, rrc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x09:
            // rrc c
            ldReg8(gpRegisters[cpuReg::BC].lo, rrc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x0A:
            // rrc d
            ldReg8(gpRegisters[cpuReg::DE].hi, rrc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x0B:
            // rrc e
            ldReg8(gpRegisters[cpuReg::DE].lo, rrc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x0C:
            // rrc h
            ldReg8(gpRegisters[cpuReg::HL].hi, rrc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x0D:
            // rrc l
            ldReg8(gpRegisters[cpuReg::HL].lo, rrc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x0E:
            // rrc (hl)
            writeMemory(address, rrc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x0F:
            // rrc a
            ldReg8(gpRegisters[cpuReg::AF].hi, rrc(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x10:
            // rl b
            ldReg8(gpRegisters[cpuReg::BC].hi, rl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x11:
            // rl c
            ldReg8(gpRegisters[cpuReg::BC].lo, rl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x12:
            // rl d
            ldReg8(gpRegisters[cpuReg::DE].hi, rl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x13:
            // rl e
            ldReg8(gpRegisters[cpuReg::DE].lo, rl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x14:
            // rl h
            ldReg8(gpRegisters[cpuReg::HL].hi, rl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x15:
            // rl l
            ldReg8(gpRegisters[cpuReg::HL].lo, rl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x16:
            // rl (hl)
            writeMemory(address, rl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x17:
            // rl a
            ldReg8(gpRegisters[cpuReg::AF].hi, rl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x18:
            // rr b
            ldReg8(gpRegisters[cpuReg::BC].hi, rr(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x19:
            // rr c
            ldReg8(gpRegisters[cpuReg::BC].lo, rr(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x1A:
            // rr d
            ldReg8(gpRegisters[cpuReg::DE].hi, rr(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x1B:
            // rr e
            ldReg8(gpRegisters[cpuReg::DE].lo, rr(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x1C:
            // rr h
            ldReg8(gpRegisters[cpuReg::HL].hi, rr(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x1D:
            // rr l
            ldReg8(gpRegisters[cpuReg::HL].lo, rr(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x1E:
            // rr (hl)
            writeMemory(address, rr(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x1F:
            // rr a
            ldReg8(gpRegisters[cpuReg::AF].hi, rr(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x20:
            // sla b
            ldReg8(gpRegisters[cpuReg::BC].hi, sla(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x21:
            // sla c
            ldReg8(gpRegisters[cpuReg::BC].lo, sla(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x22:
            // sla d
            ldReg8(gpRegisters[cpuReg::DE].hi, sla(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x23:
            // sla e
            ldReg8(gpRegisters[cpuReg::DE].lo, sla(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x24:
            // sla h
            ldReg8(gpRegisters[cpuReg::HL].hi, sla(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x25:
            // sla l
            ldReg8(gpRegisters[cpuReg::HL].lo, sla(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x26:
            // sla (hl)
            writeMemory(address, sla(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x27:
            // sla a
            ldReg8(gpRegisters[cpuReg::AF].hi, sla(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x28:
            // sra b
            ldReg8(gpRegisters[cpuReg::BC].hi, sra(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x29:
            // sra c
            ldReg8(gpRegisters[cpuReg::BC].lo, sra(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x2A:
            // sra d
            ldReg8(gpRegisters[cpuReg::DE].hi, sra(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x2B:
            // sra e
            ldReg8(gpRegisters[cpuReg::DE].lo, sra(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x2C:
            // sra h
            ldReg8(gpRegisters[cpuReg::HL].hi, sra(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x2D:
            // sra l
            ldReg8(gpRegisters[cpuReg::HL].lo, sra(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x2E:
            // sra (hl)
            writeMemory(address, sra(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x2F:
            // sra a
            ldReg8(gpRegisters[cpuReg::AF].hi, sra(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x30:
            // sll b
            ldReg8(gpRegisters[cpuReg::BC].hi, sll(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x31:
            // sll c
            ldReg8(gpRegisters[cpuReg::BC].lo, sll(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x32:
            // sll d
            ldReg8(gpRegisters[cpuReg::DE].hi, sll(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x33:
            // sll e
            ldReg8(gpRegisters[cpuReg::DE].lo, sll(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x34:
            // sll h
            ldReg8(gpRegisters[cpuReg::HL].hi, sll(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x35:
            // sll l
            ldReg8(gpRegisters[cpuReg::HL].lo, sll(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x36:
            // sll (hl)
            writeMemory(address, sll(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x37:
            // sll a
            ldReg8(gpRegisters[cpuReg::AF].hi, sll(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x38:
            // srl b
            ldReg8(gpRegisters[cpuReg::BC].hi, srl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x39:
            // srl c
            ldReg8(gpRegisters[cpuReg::BC].lo, srl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x3A:
            // srl d
            ldReg8(gpRegisters[cpuReg::DE].hi, srl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x3B:
            // srl e
            ldReg8(gpRegisters[cpuReg::DE].lo, srl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x3C:
            // srl h
            ldReg8(gpRegisters[cpuReg::HL].hi, srl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x3D:
            // srl l
            ldReg8(gpRegisters[cpuReg::HL].lo, srl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x3E:
            // srl (hl)
            writeMemory(address, srl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x3F:
            // srl a
            ldReg8(gpRegisters[cpuReg::AF].hi, srl(readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x40:
            // bit 0, b
            bit(0, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x41:
            // bit 0, c
            bit(0, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x42:
            // bit 0, d
            bit(0, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x43:
            // bit 0, e
            bit(0, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x44:
            // bit 0, h
            bit(0, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x45:
            // bit 0, l
            bit(0, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x46:
            // bit 0, (hl);
            bit(0, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x47:
            // bit 0, a
            bit(0, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x48:
            // bit 1, b
            bit(1, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x49:
            // bit 1, c
            bit(1, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x4A:
            // bit 1, d
            bit(1, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x4B:
            // bit 1, e
            bit(1, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x4C:
            // bit 1, h
            bit(1, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x4D:
            // bit 1, l
            bit(1, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x4E:
            // bit 1, (hl);
            bit(1, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x4F:
            // bit 1, a
            bit(1, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x50:
            // bit 2, b
            bit(2, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x51:
            // bit 2, c
            bit(2, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x52:
            // bit 2, d
            bit(2, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x53:
            // bit 2, e
            bit(2, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x54:
            // bit 2, h
            bit(2, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x55:
            // bit 2, l
            bit(2, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x56:
            // bit 2, (hl);
            bit(2, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x57:
            // bit 2, a
            bit(2, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x58:
            // bit 3, b
            bit(3, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x59:
            // bit 3, c
            bit(3, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x5A:
            // bit 3, d
            bit(3, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x5B:
            // bit 3, e
            bit(3, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x5C:
            // bit 3, h
            bit(3, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x5D:
            // bit 3, l
            bit(3, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x5E:
            // bit 3, (hl);
            bit(3, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x5F:
            // bit 3, a
            bit(3, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x60:
            // bit 4, b
            bit(4, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x61:
            // bit 4, c
            bit(4, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x62:
            // bit 4, d
            bit(4, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x63:
            // bit 4, e
            bit(4, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x64:
            // bit 4, h
            bit(4, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x65:
            // bit 4, l
            bit(4, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x66:
            // bit 4, (hl);
            bit(4, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x67:
            // bit 4, a
            bit(4, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x68:
            // bit 5, b
            bit(5, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x69:
            // bit 5, c
            bit(5, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x6A:
            // bit 5, d
            bit(5, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x6B:
            // bit 5, e
            bit(5, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x6C:
            // bit 5, h
            bit(5, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x6D:
            // bit 5, l
            bit(5, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x6E:
            // bit 5, (hl);
            bit(5, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x6F:
            // bit 5, a
            bit(5, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x70:
            // bit 6, b
            bit(6, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x71:
            // bit 6, c
            bit(6, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x72:
            // bit 6, d
            bit(6, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x73:
            // bit 6, e
            bit(6, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x74:
            // bit 6, h
            bit(6, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x75:
            // bit 6, l
            bit(6, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x76:
            // bit 6, (hl);
            bit(6, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x77:
            // bit 6, a
            bit(6, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x78:
            // bit 7, b
            bit(7, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x79:
            // bit 7, c
            bit(7, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x7A:
            // bit 7, d
            bit(7, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x7B:
            // bit 7, e
            bit(7, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x7C:
            // bit 7, h
            bit(7, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x7D:
            // bit 7, l
            bit(7, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x7E:
            // bit 7, (hl);
            bit(7, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x7F:
            // bit 7, a
            bit(7, readMemory(address));
            cyclesTaken = 20;
            break;
        case 0x80:
            // res 0, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x81:
            // res 0, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x82:
            // res 0, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x83:
            // res 0, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x84:
            // res 0, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x85:
            // res 0, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x86:
            // res 0, (hl)
            writeMemory(address, res(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x87:
            // res 0, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x88:
            // res 1, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x89:
            // res 1, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x8A:
            // res 1, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x8B:
            // res 1, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x8C:
            // res 1, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x8D:
            // res 1, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x8E:
            // res 1, (hl)
            writeMemory(address, res(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x8F:
            // res 1, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x90:
            // res 2, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x91:
            // res 2, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x92:
            // res 2, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x93:
            // res 2, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x94:
            // res 2, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x95:
            // res 2, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x96:
            // res 2, (hl)
            writeMemory(address, res(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x97:
            // res 2, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x98:
            // res 3, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x99:
            // res 3, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x9A:
            // res 3, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x9B:
            // res 3, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x9C:
            // res 3, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x9D:
            // res 3, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x9E:
            // res 3, (hl)
            writeMemory(address, res(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0x9F:
            // res 3, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA0:
            // res 4, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA1:
            // res 4, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA2:
            // res 4, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA3:
            // res 4, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA4:
            // res 4, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA5:
            // res 4, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA6:
            // res 4, (hl)
            writeMemory(address, res(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA7:
            // res 4, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA8:
            // res 5, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xA9:
            // res 5, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xAA:
            // res 5, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xAB:
            // res 5, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xAC:
            // res 5, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xAD:
            // res 5, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xAE:
            // res 5, (hl)
            writeMemory(address, res(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xAF:
            // res 5, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB0:
            // res 6, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB1:
            // res 6, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB2:
            // res 6, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB3:
            // res 6, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB4:
            // res 6, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB5:
            // res 6, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB6:
            // res 6, (hl)
            writeMemory(address, res(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB7:
            // res 6, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB8:
            // res 7, b
            ldReg8(gpRegisters[cpuReg::BC].hi, res(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xB9:
            // res 7, c
            ldReg8(gpRegisters[cpuReg::BC].lo, res(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xBA:
            // res 7, d
            ldReg8(gpRegisters[cpuReg::DE].hi, res(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xBB:
            // res 7, e
            ldReg8(gpRegisters[cpuReg::DE].lo, res(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xBC:
            // res 7, h
            ldReg8(gpRegisters[cpuReg::HL].hi, res(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xBD:
            // res 7, l
            ldReg8(gpRegisters[cpuReg::HL].lo, res(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xBE:
            // res 7, (hl)
            writeMemory(address, res(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xBF:
            // res 7, a
            ldReg8(gpRegisters[cpuReg::AF].hi, res(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC0:
            // set 0, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC1:
            // set 0, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC2:
            // set 0, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC3:
            // set 0, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC4:
            // set 0, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC5:
            // set 0, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC6:
            // set 0, (hl)
            writeMemory(address, set(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC7:
            // set 0, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(0, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC8:
            // set 1, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xC9:
            // set 1, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xCA:
            // set 1, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xCB:
            // set 1, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xCC:
            // set 1, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xCD:
            // set 1, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xCE:
            // set 1, (hl)
            writeMemory(address, set(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xCF:
            // set 1, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(1, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD0:
            // set 2, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD1:
            // set 2, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD2:
            // set 2, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD3:
            // set 2, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD4:
            // set 2, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD5:
            // set 2, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD6:
            // set 2, (hl)
            writeMemory(address, set(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD7:
            // set 2, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(2, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD8:
            // set 3, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xD9:
            // set 3, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xDA:
            // set 3, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xDB:
            // set 3, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xDC:
            // set 3, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xDD:
            // set 3, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xDE:
            // set 3, (hl)
            writeMemory(address, set(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xDF:
            // set 3, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(3, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE0:
            // set 4, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE1:
            // set 4, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE2:
            // set 4, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE3:
            // set 4, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE4:
            // set 4, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE5:
            // set 4, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE6:
            // set 4, (hl)
            writeMemory(address, set(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE7:
            // set 4, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(4, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE8:
            // set 5, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xE9:
            // set 5, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xEA:
            // set 5, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xEB:
            // set 5, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xEC:
            // set 5, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xED:
            // set 5, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xEE:
            // set 5, (hl)
            writeMemory(address, set(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xEF:
            // set 5, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(5, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF0:
            // set 6, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF1:
            // set 6, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF2:
            // set 6, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF3:
            // set 6, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF4:
            // set 6, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF5:
            // set 6, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF6:
            // set 6, (hl)
            writeMemory(address, set(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF7:
            // set 6, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(6, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF8:
            // set 7, b
            ldReg8(gpRegisters[cpuReg::BC].hi, set(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xF9:
            // set 7, c
            ldReg8(gpRegisters[cpuReg::BC].lo, set(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xFA:
            // set 7, d
            ldReg8(gpRegisters[cpuReg::DE].hi, set(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xFB:
            // set 7, e
            ldReg8(gpRegisters[cpuReg::DE].lo, set(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xFC:
            // set 7, h
            ldReg8(gpRegisters[cpuReg::HL].hi, set(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xFD:
            // set 7, l
            ldReg8(gpRegisters[cpuReg::HL].lo, set(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xFE:
            // set 7, (hl)
            writeMemory(address, set(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        case 0xFF:
            // set 7, a
            ldReg8(gpRegisters[cpuReg::AF].hi, set(7, readMemory(address)));
            cyclesTaken = 23;
            break;
        default:
            std::stringstream ss;
            ss << "Unimplemented" << indexPrefix << " bit opcode: 0x" << std::hex << (int) opcode << std::endl;
            throw Z80Exception(ss.str());
    }

}