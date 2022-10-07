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

    cyclesTaken = 0;
    originalProgramCounterValue = programCounter;
    executedInstructionName = "";

    // TODO handle interrupts
    if (enableInterrupts) {
        iff1 = iff2 = true;
        enableInterrupts = false;
    }

    unsigned char opcode = NB();
    unsigned char upperOpcode; // Use for 2-byte instructions

#ifdef VERBOSE_MODE
    std::string prefix = "";
    unsigned char displayOpcode = opcode;
#endif

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
            memory->write(gpRegisters[cpuReg::BC].whole, gpRegisters[cpuReg::AF].hi);
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
            rlc(gpRegisters[cpuReg::AF].hi);
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
            ldReg8(gpRegisters[cpuReg::AF].hi, memory->read(gpRegisters[cpuReg::BC].whole));
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
            rrc(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x10:
            // djnz, d
            djnz(programCounter, NB());
            break;
        case 0x11:
            // ld de, nn
            ldReg16(gpRegisters[cpuReg::DE].whole, build16BitNumber(), false);
            cyclesTaken = 10;
            break;
        case 0x12:
            // ld (de), a
            memory->write(gpRegisters[cpuReg::DE].whole, gpRegisters[cpuReg::AF].hi);
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
            rl(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x18:
            // jr d
            jr(NB());
            break;
        case 0x19:
            // add hl, de
            add16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 11;
            break;
        case 0X1A:
            // ld a, (de)
            ldReg8(gpRegisters[cpuReg::AF].hi, memory->read(gpRegisters[cpuReg::DE].whole));
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
            rr(gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x20:
            // jr nz, d
            jrCondition(JPCondition::NZ, NB());
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
            jrCondition(JPCondition::Z, NB());
            break;
        case 0x29:
            // add hl, hl
            add16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].whole);
            cyclesTaken = 11;
            break;
        case 0x2A:
            // ld hl, (nn)
            ldReg16(gpRegisters[cpuReg::HL].whole, memory->read(build16BitNumber()), false);
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
            jrCondition(JPCondition::NC, NB());
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
        case 0x34:
            // inc (hl)
            memory->write(gpRegisters[cpuReg::HL].whole, getInc8BitValue(memory->read(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 11;
            break;
        case 0x35:
            // dec (hl)
            memory->write(gpRegisters[cpuReg::HL].whole, getDec8BitValue(memory->read(gpRegisters[cpuReg::HL].whole)));
            cyclesTaken = 11;
            break;
        case 0x36:
            // ld (hl), n
            memory->write(gpRegisters[cpuReg::HL].whole, NB());
            cyclesTaken = 10;
            break;
        case 0x37:
            // scf
            setFlag(CPUFlag::carry, true);
            setFlag(CPUFlag::subtract, false);
            setFlag(CPUFlag::halfCarry, false);
            cyclesTaken = 4;
            break;
        case 0x38:
            // jr c, d
            jrCondition(JPCondition::C, NB());
            break;
        case 0x39:
            // add hl, sp
            add16Bit(gpRegisters[cpuReg::HL].whole, stackPointer);
            cyclesTaken = 11;
            break;
        case 0x3A:
            // ld a, (nn)
            ldReg8(gpRegisters[cpuReg::AF].hi, memory->read(build16BitNumber()));
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
            setFlag(CPUFlag::carry, !getFlag(CPUFlag::carry));
            setFlag(CPUFlag::halfCarry, !getFlag(CPUFlag::halfCarry));
            setFlag(CPUFlag::subtract, false);
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
            ldReg8(gpRegisters[cpuReg::BC].hi, memory->read(gpRegisters[cpuReg::HL].whole));
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
            ldReg8(gpRegisters[cpuReg::BC].lo, memory->read(gpRegisters[cpuReg::HL].whole));
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
            ldReg8(gpRegisters[cpuReg::DE].hi, memory->read(gpRegisters[cpuReg::HL].whole));
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
            ldReg8(gpRegisters[cpuReg::DE].lo, memory->read(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x5F:
            // ld e, a
            ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x63:
            // ld h, e
            ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x6F:
            // ld l, a
            ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0x73:
            // ld (hl), e
            memory->write(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 7;
            break;
        case 0x77:
            // ld (hl), a
            memory->write(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::AF].hi);
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
            ldReg8(gpRegisters[cpuReg::AF].hi, memory->read(gpRegisters[cpuReg::HL].whole));
            cyclesTaken = 7;
            break;
        case 0x80:
            // add a, b
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
            cyclesTaken = 7;
            break;
        case 0x83:
            // add a, e
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
            break;
        case 0x87:
            // add a, a
            add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
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
        case 0xA7:
            // and a
            and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0xAF:
            // xor a
            exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 4;
            break;
        case 0xB1:
            // or c
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
            cyclesTaken = 4;
            break;
        case 0xB3:
            // or e
            or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
            cyclesTaken = 4;
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
        case 0xC5:
            // push bc
            pushStack(gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 11;
            break;
        case 0xC9:
            // ret
            programCounter = popStack16();
            cyclesTaken = 10;
            break;
        case 0xCC:
            // call z, nn
            call(build16BitNumber(), getFlag(CPUFlag::zero));
            break;
        case 0xCD:
            // call nn
            call(build16BitNumber());
            break;
        case 0xD1:
            // pop de
            gpRegisters[cpuReg::DE].whole = popStack16();
            cyclesTaken = 10;
            break;
        case 0xD3:
            // out (n) a
            writeIOPort(NB(), gpRegisters[cpuReg::AF].hi);
            cyclesTaken = 11;
            break;
        case 0xD5:
            // push de
            pushStack(gpRegisters[cpuReg::DE].whole);
            cyclesTaken = 11;
            break;
        case 0xDC:
            // call c, nn
            call(build16BitNumber(), getFlag(CPUFlag::carry));
            break;
        case 0xDB:
            // in a, (n)
            ldReg8(gpRegisters[cpuReg::AF].hi, readIOPort(NB()));
            cyclesTaken = 11;
            break;
        case 0xE1:
            // pop hl
            gpRegisters[cpuReg::HL].whole = popStack16();
            cyclesTaken = 10;
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
        case 0xEC:
            // call pe, nn
            call(build16BitNumber(), getFlag(CPUFlag::overflow));
            break;
        case 0xED:
            upperOpcode = NB();
            extendedOpcodes(upperOpcode);

#ifdef VERBOSE_MODE
            prefix = "ED";
            displayOpcode = upperOpcode;
#endif

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
        case 0xFA:
            // jp m, nn
            jpCondition(JPCondition::M);
            break;
        case 0xFB:
            // ei
            enableInterrupts = true; // Interrupts should be re-enabled when executing the next instruction
            cyclesTaken = 4;
            break;
        case 0xFD:
            upperOpcode = NB();
            iyOpcodes(upperOpcode);

#ifdef VERBOSE_MODE
            prefix = "IY";
            displayOpcode = upperOpcode;
#endif
            break;
        case 0xFE:
            // cp n
            compare8Bit(NB());
            cyclesTaken = 7;
            break;
        default:
            state = CPUState::Error;

            std::cout << "Error: Unknown opcode: 0x" << std::hex << (int) opcode << "-  At PC: 0x"
                      << (int) originalProgramCounterValue << std::endl;
            break;
    }

    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(opcode, 0x0, 0x0);
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
 * Returns the signed byte located at the program counter in memory and increments the program counter
 * @return
 */
signed char CPUZ80::signedNB() {
    return static_cast<signed char>(memory->read(programCounter++));
}

/**
 * [CPUZ80::extendedOpcodes Handles extended opcodes]
 */
void CPUZ80::extendedOpcodes(unsigned char opcode) {

    switch (opcode) {
        case 0x42:
            // sbc hl, bc
            sbc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].whole);
            cyclesTaken = 15;
            break;
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
        case 0xB0:
            // ldir
            ldir();
            break;
        case 0xB3:
            // otir
            otir();
            break;
        default:
            state = CPUState::Error;

#ifdef VERBOSE_MODE
            std::cout << "Error: Unknown extended opcode: 0x" << std::hex << (int) opcode << std::endl;
#endif

            break;
    }

    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(0xED, opcode, 0x0);
    }
}

void CPUZ80::iyOpcodes(unsigned char opcode) {

    switch (opcode) {
        case 0x66:
            // ld h, (iy+d)
            ldReg8(gpRegisters[cpuReg::HL].hi, memory->read(gpRegisters[cpuReg::IY].whole + signedNB()));
            cyclesTaken = 19;
            break;
            break;
        case 0x6E:
            // ld l, (iy+d)
            ldReg8(gpRegisters[cpuReg::HL].lo, memory->read(gpRegisters[cpuReg::IY].whole + signedNB()));
            cyclesTaken = 19;
            break;
        case 0x7E:
            // ld a, (iy+d)
            ldReg8(gpRegisters[cpuReg::AF].hi, memory->read(gpRegisters[cpuReg::IY].whole + signedNB()));
            cyclesTaken = 19;
            break;
        case 0xE1:
            // pop iy
            ldReg16(gpRegisters[cpuReg::IY].whole, popStack16(), false);
            cyclesTaken = 14;
            break;
        default:
            state = CPUState::Error;

#ifdef VERBOSE_MODE
            std::cout << "Error: Unknown iy opcode: 0x" << std::hex << (int) opcode << std::endl;
#endif

            break;
    }

    if (executedInstructionName.empty()) {
        executedInstructionName = getInstructionName(0xFD, opcode, 0x0);
    }

}

/**
 * [logCPUState Log the CPU's current state to the console]
 */
void CPUZ80::logCPUState(unsigned char opcode, std::string prefix) {
    std::cout << std::uppercase << "PC: 0x" << std::hex << (int) originalProgramCounterValue << " Opcode: 0x" << prefix
              << (int) opcode << " - " << Utils::padString(executedInstructionName, 10) << " - Registers: AF=0x" << (int) gpRegisters[cpuReg::AF].whole << " BC=0x"
              << (int) gpRegisters[cpuReg::BC].whole << " DE=0x" << (int) gpRegisters[cpuReg::DE].whole << " HL=0x"
              << (int) gpRegisters[cpuReg::HL].whole << " IX=0x" << (int) gpRegisters[cpuReg::IX].whole << " IY=0x"
              << (int) gpRegisters[cpuReg::IY].whole << " SP=0x" << (int) stackPointer << std::endl;
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
            return !getFlag(CPUFlag::overflow);
        case JPCondition::PE:
            return getFlag(CPUFlag::overflow);
        case JPCondition::P:
            return !getFlag(CPUFlag::sign);
        case JPCondition::M:
            return getFlag(CPUFlag::sign);
        default:
            std::cout<<"Unhandled jump condition"<<std::endl;
            throw std::exception(); // TODO build a custom exception class for storing and displaying error messages
    }
}

void CPUZ80::pushStack(unsigned char value) {
    memory->write(stackPointer--, value);
}

void CPUZ80::pushStack(unsigned short value) {
    unsigned char hi = value >> 8;
    unsigned char lo = value & 0x00FF;
    pushStack(hi);
    pushStack(lo);
}

unsigned char CPUZ80::popStack() {
    return memory->read(++stackPointer);
}

unsigned short CPUZ80::popStack16() {
    return (popStack() + (popStack()<<8));
}