//
// Created by Peter Savory on 09/06/2023.
//

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"
#include <Exceptions.h>

void CPUZ80::opcodeHandlerInvalid() {
    // TODO display a more detailed error message like before
    throw Z80Exception("Invalid Opcode");
    /**
     *             std::stringstream ss;
            ss << "Unimplemented opcode: 0x" << std::hex << (int) opcode << "-  At PC: 0x"
                      << (int) originalProgramCounterValue << std::endl;
            throw Z80Exception(ss.str());
     */
}

void CPUZ80::standardOpcodeHandler0x00() {
    // nop
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x01() {
    // ld bc, nn
    ldReg16(gpRegisters[cpuReg::BC].whole, build16BitNumber(), false);
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0x02() {
    // ld (bc), a
    writeMemory(gpRegisters[cpuReg::BC].whole, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x03() {
    // inc bc
    inc16Bit(gpRegisters[cpuReg::BC].whole);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0x04() {
    // inc b
    inc8Bit(gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x05() {
    // dec b
    dec8Bit(gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x06() {
    // ld b, n
    ldReg8(gpRegisters[cpuReg::BC].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x07() {
    // rlca
    rlca();
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x08() {
    // ex af, af'
    exchange16Bit(gpRegisters[cpuReg::AF].whole, gpRegisters[cpuReg::AFS].whole);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x09() {
    // add hl, bc
    add16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].whole);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0x0A() {
    // ld a, (bc)
    ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::BC].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x0B() {
    // dec bc
    dec16Bit(gpRegisters[cpuReg::BC].whole);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0x0C() {
    // inc c
    inc8Bit(gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x0D() {
    // dec c
    dec8Bit(gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x0E() {
    // ld c, n
    ldReg8(gpRegisters[cpuReg::BC].lo, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x0F() {
    // rrca
    rrca();
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x10() {
    // djnz, d
    djnz();
}

void CPUZ80::standardOpcodeHandler0x11() {
    // ld de, nn
    ldReg16(gpRegisters[cpuReg::DE].whole, build16BitNumber(), false);
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0x12() {
    // ld (de), a
    writeMemory(gpRegisters[cpuReg::DE].whole, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x13() {
    // inc de
    inc16Bit(gpRegisters[cpuReg::DE].whole);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0x14() {
    // inc d
    inc8Bit(gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x15() {
    // dec d
    dec8Bit(gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x16() {
    // ld d, n
    ldReg8(gpRegisters[cpuReg::DE].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x17() {
    // rla
    rla();
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x18() {
    // jr d
    jr();
}

void CPUZ80::standardOpcodeHandler0x19() {
    // add hl, de
    add16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].whole);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0x1A() {
    // ld a, (de)
    ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::DE].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x1B() {
    // dec de
    dec16Bit(gpRegisters[cpuReg::DE].whole);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0x1C() {
    // inc e
    inc8Bit(gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x1D() {
    // dec e
    dec8Bit(gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x1E() {
    // ld e, n
    ldReg8(gpRegisters[cpuReg::DE].lo, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x1F() {
    // rra
    rra();
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x20() {
    // jr nz, d
    jrCondition(JPCondition::NZ);
}

void CPUZ80::standardOpcodeHandler0x21() {
    // ld hl, nn
    ldReg16(gpRegisters[cpuReg::HL].whole, build16BitNumber(), false);
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0x22() {
    // ld (nn), hl
    store(build16BitNumber(), gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 16;
}

void CPUZ80::standardOpcodeHandler0x23() {
    // inc hl
    inc16Bit(gpRegisters[cpuReg::HL].whole);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0x24() {
    // inc h
    inc8Bit(gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x25() {
    // dec h
    dec8Bit(gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x26() {
    // ld h, n
    ldReg8(gpRegisters[cpuReg::HL].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x27() {
    // daa
    daa(gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x28() {
    // jr z, d
    jrCondition(JPCondition::Z);
}

void CPUZ80::standardOpcodeHandler0x29() {
    // add hl, hl
    add16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].whole);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0x2A() {
    // ld hl, (nn)
    ldReg16(gpRegisters[cpuReg::HL].whole, readMemory16Bit(build16BitNumber()), false);
    cyclesTaken = 16;
}

void CPUZ80::standardOpcodeHandler0x2B() {
    // dec hl
    dec16Bit(gpRegisters[cpuReg::HL].whole);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0x2C() {
    // inc l
    inc8Bit(gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x2D() {
    // dec l
    dec8Bit(gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x2E() {
    // ld l, n
    ldReg8(gpRegisters[cpuReg::HL].lo, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x2F() {
    // cpl
    cpl(gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x30() {
    // jr nc, d
    jrCondition(JPCondition::NC);
}

void CPUZ80::standardOpcodeHandler0x31() {
    // ld sp, nn
    ldReg16(stackPointer, build16BitNumber(), false);
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0x32() {
    // ld (nn), a
    writeMemory(build16BitNumber(), gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 13;
}

void CPUZ80::standardOpcodeHandler0x33() {
    // inc sp
    inc16Bit(stackPointer);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0x34() {
    // inc (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, getInc8BitValue(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0x35() {
    // dec (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, getDec8BitValue(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0x36() {
    // ld (hl), n
    writeMemory(gpRegisters[cpuReg::HL].whole, NB());
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0x37() {
    // scf
    setFlag(CPUFlag::carry, true);
    setFlag(CPUFlag::subtractNegative, false);
    setFlag(CPUFlag::halfCarry, false);

    // Set XF/YF flags accordingly
    gpRegisters[cpuReg::AF].lo |= gpRegisters[cpuReg::AF].hi & 0x28;

    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x38() {
    // jr c, d
    jrCondition(JPCondition::C);
}

void CPUZ80::standardOpcodeHandler0x39() {
    // add hl, sp
    add16Bit(gpRegisters[cpuReg::HL].whole, stackPointer);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0x3A() {
    // ld a, (nn)
    ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(build16BitNumber()));
    cyclesTaken = 13;
}

void CPUZ80::standardOpcodeHandler0x3B() {
    // dec sp
    dec16Bit(stackPointer);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0x3C() {
    // inc a
    inc8Bit(gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x3D() {
    // dec a
    dec8Bit(gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x3E() {
    // ld a, n
    ldReg8(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x3F() {
    // ccf
    ccf();
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x40() {
    // ld b, b (Why does this even exist?)
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x41() {
    // ld b, c
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x42() {
    // ld b, d
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x43() {
    // ld b, e
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x44() {
    // ld b, h
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x45() {
    // ld b, l
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x46() {
    // ld b, (hl)
    ldReg8(gpRegisters[cpuReg::BC].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x47() {
    // ld b, a
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x48() {
    // ld c, b
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x49() {
    // ld c, c
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x4A() {
    // ld c, d
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x4B() {
    // ld c, e
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x4C() {
    // ld c, h
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x4D() {
    // ld c, l
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x4E() {
    // ld c, (hl)
    ldReg8(gpRegisters[cpuReg::BC].lo, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x4F() {
    // ld c, a
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x50() {
    // ld d, b
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x51() {
    // ld d, c
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x52() {
    // ld d, d
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x53() {
    // ld d, e
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x54() {
    // ld d, h
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x55() {
    // ld d, l
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x56() {
    // ld d, (hl)
    ldReg8(gpRegisters[cpuReg::DE].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x57() {
    // ld d, a
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x58() {
    // ld e, b
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x59() {
    // ld e, c
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x5A() {
    // ld e, d
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x5B() {
    // ld e, e
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x5C() {
    // ld e, h
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x5D() {
    // ld e, l
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x5E() {
    // ld e, (hl)
    ldReg8(gpRegisters[cpuReg::DE].lo, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x5F() {
    // ld e, a
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x60() {
    // ld h, b
    ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x61() {
    // ld h, c
    ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x62() {
    // ld h, d
    ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x63() {
    // ld h, e
    ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x64() {
    // ld h, h
    ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x65() {
    // ld h, l
    ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x66() {
    // ld h, (hl)
    ldReg8(gpRegisters[cpuReg::HL].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x67() {
    // ld h, a
    ldReg8(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x68() {
    // ld l, b
    ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x69() {
    // ld l, c
    ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x6A() {
    // ld l, d
    ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x6B() {
    // ld l, e
    ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x6C() {
    // ld l, h
    ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x6D() {
    // ld l, l
    ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x6E() {
    // ld l, (hl)
    ldReg8(gpRegisters[cpuReg::HL].lo, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x6F() {
    // ld l, a
    ldReg8(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x70() {
    // ld (hl), b
    writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x71() {
    // ld (hl), c
    writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x72() {
    // ld (hl), d
    writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x73() {
    // ld (hl), e
    writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x74() {
    // ld (hl), h
    writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x75() {
    // ld (hl), l
    writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x76() {
    // halt
    state = CPUState::Halt;
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x77() {
    // ld (hl), a
    writeMemory(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x78() {
    // ld a, b
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x79() {
    // ld a, c
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x7A() {
    // ld a, d
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x7B() {
    // ld a, e
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x7C() {
    // ld a, h
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x7D() {
    // ld a, l
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x7E() {
    // ld a (hl)
    ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x7F() {
    // ld a, a
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x80() {
    // add a, b
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x81() {
    // add a, c
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x82() {
    // add a, d
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x83() {
    // add a, e
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x84() {
    // add a, h
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x85() {
    // add a, l
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x86() {
    // add a, (hl)
    add8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x87() {
    // add a, a
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x88() {
    // adc a, b
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x89() {
    // adc a, c
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x8A() {
    // adc a, d
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x8B() {
    // adc a, e
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x8C() {
    // adc a, h
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x8D() {
    // adc a, l
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x8E() {
    // adc a, (hl)
    adc8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x8F() {
    // adc a, a
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x90() {
    // sub b
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x91() {
    // sub c
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x92() {
    // sub d
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x93() {
    // sub e
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x94() {
    // sub h
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x95() {
    // sub l
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x96() {
    // sub (hl)
    sub8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x97() {
    // sub a
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x98() {
    // sbc a, b
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x99() {
    // sbc a, c
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x9A() {
    // sbc a, d
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x9B() {
    // sbc a, e
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x9C() {
    // sbc a, h
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x9D() {
    // sbc a, l
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0x9E() {
    // sbc a, (hl)
    sbc8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0x9F() {
    // sbc a, a
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA0() {
    // and b
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA1() {
    // and c
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA2() {
    // and d
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA3() {
    // and e
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA4() {
    // and h
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA5() {
    // and l
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA6() {
    // and (hl)
    and8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA7() {
    // and a
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA8() {
    // xor b
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xA9() {
    // xor c
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xAA() {
    // xor d
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xAB() {
    // xor e
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xAC() {
    // xor h
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xAD() {
    // xor l
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xAE() {
    // xor (hl)
    exclusiveOr(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xAF() {
    // xor a
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB0() {
    // or b
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB1() {
    // or c
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB2() {
    // or d
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB3() {
    // or e
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB4() {
    // or h
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB5() {
    // or l
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB6() {
    // or (hl)
    or8Bit(gpRegisters[cpuReg::AF].hi, readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xB7() {
    // or a
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB8() {
    // cp b
    compare8Bit(gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xB9() {
    // cp c
    compare8Bit(gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xBA() {
    // cp d
    compare8Bit(gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xBB() {
    // cp e
    compare8Bit(gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xBC() {
    // cp h
    compare8Bit(gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xBD() {
    // cp l
    compare8Bit(gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xBE() {
    // cp (hl)
    compare8Bit(readMemory(gpRegisters[cpuReg::HL].whole));
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xBF() {
    // cp a
    compare8Bit(gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xC0() {
    // ret nz
    retCondition(JPCondition::NZ);
}

void CPUZ80::standardOpcodeHandler0xC1() {
    //pop bc
    gpRegisters[cpuReg::BC].whole = popStack16();
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0xC2() {
    jpCondition(JPCondition::NZ);
}

void CPUZ80::standardOpcodeHandler0xC3() {
    // jp nn
    jpImm();
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0xC4() {
    // call nz, nn
    callCondition(JPCondition::NZ);
}

void CPUZ80::standardOpcodeHandler0xC5() {
    // push bc
    pushStack(gpRegisters[cpuReg::BC].whole);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0xC6() {
    // add a, n
    add8Bit(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xC7() {
    // rst 00h
    rst(0x00);
}

void CPUZ80::standardOpcodeHandler0xC8() {
    // ret z
    retCondition(JPCondition::Z);
}

void CPUZ80::standardOpcodeHandler0xC9() {
    // ret
    programCounter = popStack16();
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0xCA() {
    // jp z, nn
    jpCondition(JPCondition::Z);
}

void CPUZ80::standardOpcodeHandler0xCB() {
    // bit opcodes
    bitOpcodes();
}

void CPUZ80::standardOpcodeHandler0xCC() {
    // call z, nn
    callCondition(JPCondition::Z);
}

void CPUZ80::standardOpcodeHandler0xCD() {
    // call nn
    call(build16BitNumber());
}

void CPUZ80::standardOpcodeHandler0xCE() {
    // adc a, n
    adc8Bit(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xCF() {
    // rst 08h
    rst(0x08);
}

void CPUZ80::standardOpcodeHandler0xD0() {
    // ret nc
    retCondition(JPCondition::NC);
}

void CPUZ80::standardOpcodeHandler0xD1() {
    // pop de
    gpRegisters[cpuReg::DE].whole = popStack16();
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0xD2() {
    // jp nc, nn
    jpCondition(JPCondition::NC);
}

void CPUZ80::standardOpcodeHandler0xD3() {
    // out (n) a
    portOut(NB(), gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0xD4() {
    // call nc, nn
    call(build16BitNumber(), !getFlag(CPUFlag::carry));
}

void CPUZ80::standardOpcodeHandler0xD5() {
    // push de
    pushStack(gpRegisters[cpuReg::DE].whole);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0xD6() {
    // sub n
    sub8Bit(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xD7() {
    // rst 10h
    rst(0x10);
}

void CPUZ80::standardOpcodeHandler0xD8() {
    // ret c
    retCondition(JPCondition::C);
}

void CPUZ80::standardOpcodeHandler0xD9() {
    // exx
    exchange16Bit(gpRegisters[cpuReg::BC].whole, gpRegisters[cpuReg::BCS].whole);
    exchange16Bit(gpRegisters[cpuReg::DE].whole, gpRegisters[cpuReg::DES].whole);
    exchange16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HLS].whole);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xDA() {
    // jp c, nn
    jpCondition(JPCondition::C);
}

void CPUZ80::standardOpcodeHandler0xDB() {
    // in a, (n)
    readPortToRegister(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0xDC() {
    // call c, nn
    callCondition(JPCondition::C);
}

void CPUZ80::standardOpcodeHandler0xDD() {
    // ix opcodes
    indexOpcodes(cpuReg::IX);
}

void CPUZ80::standardOpcodeHandler0xDE() {
    // sbc a, n
    sbc8Bit(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xDF() {
    // rst 18h
    rst(0x18);
}

void CPUZ80::standardOpcodeHandler0xE0() {
    // ret po
    retCondition(JPCondition::PO);
}

void CPUZ80::standardOpcodeHandler0xE1() {
    // pop hl
    gpRegisters[cpuReg::HL].whole = popStack16();
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0xE2() {
    // jp po, nn
    jpCondition(JPCondition::PO);
}

void CPUZ80::standardOpcodeHandler0xE3() {
    // ex (sp), hl
    exStack(gpRegisters[cpuReg::HL].whole);
}

void CPUZ80::standardOpcodeHandler0xE4() {
    // call po, nn
    call(build16BitNumber(), !getFlag(CPUFlag::overflowParity));
}

void CPUZ80::standardOpcodeHandler0xE5() {
    // push hl
    pushStack(gpRegisters[cpuReg::HL].whole);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0xE6() {
    // and n
    and8Bit(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xE7() {
    // rst 20h
    rst(0x20);
}

void CPUZ80::standardOpcodeHandler0xE8() {
    // ret pe
    retCondition(JPCondition::PE);
}

void CPUZ80::standardOpcodeHandler0xE9() {
    // jp (hl) - This is not indirect, though the notation implies so
    programCounter = gpRegisters[cpuReg::HL].whole;
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xEA() {
    // jp pe, nn
    jpCondition(JPCondition::PE);
}

void CPUZ80::standardOpcodeHandler0xEB() {
    // ex de, hl
    exchange16Bit(gpRegisters[cpuReg::DE].whole, gpRegisters[cpuReg::HL].whole);
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xEC() {
    // call pe, nn
    callCondition(JPCondition::PE);
}

void CPUZ80::standardOpcodeHandler0xED() {
    extendedOpcodes();
}

void CPUZ80::standardOpcodeHandler0xEE() {
    // xor n
    exclusiveOr(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xEF() {
    // rst 28h
    rst(0x28);
}

void CPUZ80::standardOpcodeHandler0xF0() {
    // ret p
    retCondition(JPCondition::P);
}

void CPUZ80::standardOpcodeHandler0xF1() {
    // pop af
    gpRegisters[cpuReg::AF].whole = popStack16();
    cyclesTaken = 10;
}

void CPUZ80::standardOpcodeHandler0xF2() {
    // jp p, nn
    jpCondition(JPCondition::P);
    cyclesTaken  = 10;
}

void CPUZ80::standardOpcodeHandler0xF3() {
    // di
    iff1 = iff2 = false;
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xF4() {
    // call p, nn
    callCondition(JPCondition::P);
}

void CPUZ80::standardOpcodeHandler0xF5() {
    // push af
    pushStack(gpRegisters[cpuReg::AF].whole);
    cyclesTaken = 11;
}

void CPUZ80::standardOpcodeHandler0xF6() {
    // or n
    or8Bit(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xF7() {
    // rst 30h
    rst(0x30);
}

void CPUZ80::standardOpcodeHandler0xF8() {
    // ret m
    retCondition(JPCondition::M);
}

void CPUZ80::standardOpcodeHandler0xF9() {
    // ld sp, hl
    ldReg16(stackPointer, gpRegisters[cpuReg::HL].whole, false);
    cyclesTaken = 6;
}

void CPUZ80::standardOpcodeHandler0xFA() {
    // jp m, nn
    jpCondition(JPCondition::M);
}

void CPUZ80::standardOpcodeHandler0xFB() {
    // ei
    enableInterrupts = true; // Interrupts should be re-enabled when executing the next instruction
    cyclesTaken = 4;
}

void CPUZ80::standardOpcodeHandler0xFC() {
    // call m, nn
    callCondition(JPCondition::M);
}

void CPUZ80::standardOpcodeHandler0xFD() {
    indexOpcodes(cpuReg::IY);
}

void CPUZ80::standardOpcodeHandler0xFE() {
    // cp n
    compare8Bit(NB());
    cyclesTaken = 7;
}

void CPUZ80::standardOpcodeHandler0xFF() {
    // rst 38h
    rst(0x38);
}