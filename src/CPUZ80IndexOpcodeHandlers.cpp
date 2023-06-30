//
// Created by Peter Savory on 09/06/2023.
//

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"
#include <Exceptions.h>

void CPUZ80::invalidIndexOpcodeHandler() {
    // TODO implement a more detailed/useful message
    throw Z80Exception("Invalid Index opcode");
}


void CPUZ80::indexOpcodeHandler0x04() {
    // inc b
    inc8Bit(gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x05() {
    // dec b
    dec8Bit(gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x06() {
    // ld b, n
    ldReg8(gpRegisters[cpuReg::BC].hi, NB());
    cyclesTaken = 11;
}

void CPUZ80::indexOpcodeHandler0x09() {
    // add ix, bc
    add16Bit(gpRegisters[indexRegisterForCurrentOpcode].whole, gpRegisters[cpuReg::BC].whole);
    cyclesTaken = 15;
}

void CPUZ80::indexOpcodeHandler0x0C() {
    // inc c
    inc8Bit(gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x0D() {
    // dec c
    dec8Bit(gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x0E() {
    // ld c, n
    ldReg8(gpRegisters[cpuReg::BC].lo, NB());
    cyclesTaken = 11;
}

void CPUZ80::indexOpcodeHandler0x14() {
    // inc d
    inc8Bit(gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x15() {
    // dec d
    dec8Bit(gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x16() {
    // ld d, n
    ldReg8(gpRegisters[cpuReg::DE].hi, NB());
    cyclesTaken = 11;
}

void CPUZ80::indexOpcodeHandler0x19() {
    // add ix,de
    add16Bit(gpRegisters[indexRegisterForCurrentOpcode].whole, gpRegisters[cpuReg::DE].whole);
    cyclesTaken = 15;
}

void CPUZ80::indexOpcodeHandler0x1C() {
    // inc e
    inc8Bit(gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x1D() {
    // dec e
    dec8Bit(gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x1E() {
    // ld e, n
    ldReg8(gpRegisters[cpuReg::DE].lo, NB());
    cyclesTaken = 11;
}

void CPUZ80::indexOpcodeHandler0x21() {
    // ld ix, nn
    ldReg16(gpRegisters[indexRegisterForCurrentOpcode].whole, build16BitNumber());
    cyclesTaken = 14;
}

void CPUZ80::indexOpcodeHandler0x22() {
    // ld (nn), ix
    unsigned short address = build16BitNumber();
    writeMemory(address, gpRegisters[indexRegisterForCurrentOpcode].whole);
    gpRegisters[cpuReg::WZ].whole = address + 1;
    cyclesTaken = 20;
}

void CPUZ80::indexOpcodeHandler0x23() {
    // inc ix
    inc16Bit(gpRegisters[indexRegisterForCurrentOpcode].whole);
    cyclesTaken = 10;
}

void CPUZ80::indexOpcodeHandler0x24() {
    // inc ixh
    inc8Bit(gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x25() {
    // dec ixh
    dec8Bit(gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x26() {
    // ld ixh, n
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].hi, NB());
    cyclesTaken = 11;
}

void CPUZ80::indexOpcodeHandler0x29() {
    // add ix, sp
    add16Bit(gpRegisters[indexRegisterForCurrentOpcode].whole, gpRegisters[indexRegisterForCurrentOpcode].whole);
    cyclesTaken = 15;
}

void CPUZ80::indexOpcodeHandler0x2A() {
    // ld ix, (nn)
    unsigned short address = build16BitNumber();
    ldReg16(gpRegisters[indexRegisterForCurrentOpcode].whole, readMemory16Bit(address));
    gpRegisters[cpuReg::WZ].whole = address + 1;
    cyclesTaken = 20;
}

void CPUZ80::indexOpcodeHandler0x2B() {
    // dec ix
    dec16Bit(gpRegisters[indexRegisterForCurrentOpcode].whole);
    cyclesTaken = 10;
}

void CPUZ80::indexOpcodeHandler0x2C() {
    // inc ixl
    inc8Bit(gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x2D() {
    // dec ixl
    dec8Bit(gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x2E() {
    // ld ixl, n
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].lo, NB());
    cyclesTaken = 11;
}

void CPUZ80::indexOpcodeHandler0x34() {
    // inc (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, getInc8BitValue(readMemory(address)));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 23;
}

void CPUZ80::indexOpcodeHandler0x35() {
    // dec (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, getDec8BitValue(readMemory(address)));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 23;
}

void CPUZ80::indexOpcodeHandler0x36() {
    // ld (ix+d), n
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, NB());
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x39() {
    // add ix, sp
    add16Bit(gpRegisters[indexRegisterForCurrentOpcode].whole, stackPointer);
    cyclesTaken = 15;
}

void CPUZ80::indexOpcodeHandler0x3C() {
    // inc a
    inc8Bit(gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x3D() {
    // dec a
    dec8Bit(gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x3E() {
    // ld a, n
    ldReg8(gpRegisters[cpuReg::AF].hi, NB());
    cyclesTaken = 11;
}

void CPUZ80::indexOpcodeHandler0x40() {
    // ld b, b
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x41() {
    // ld b, c
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x42() {
    // ld b, d
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x43() {
    // ld b, e
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x44() {
    // ld b, ixh
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x45() {
    // ld b, ixl
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x46() {
    // ld b, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    ldReg8(gpRegisters[cpuReg::BC].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x47() {
    // ld b, a
    ldReg8(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x48() {
    // ld c, b
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x49() {
    // ld c, c
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x4A() {
    // ld c, d
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x4B() {
    // ld c, e
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x4C() {
    // ld c, ixh
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x4D() {
    // ld c, ixl
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x4E() {
    // ld c, (ix+d)
    ldReg8(gpRegisters[cpuReg::BC].lo, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole)));
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x4F() {
    // ld c, a
    ldReg8(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x50() {
    // ld d, b
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x51() {
    // ld d, c
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x52() {
    // ld d, d
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x53() {
    // ld d, e
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x54() {
    // ld d, ixh
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x55() {
    // ld d, ixl
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x56() {
    // ld d, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    ldReg8(gpRegisters[cpuReg::DE].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x57() {
    // ld d, a
    ldReg8(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x58() {
    // ld e, b
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x59() {
    // ld e, c
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x5A() {
    // ld e, d
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x5B() {
    // ld e, e
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x5C() {
    // ld e, ixh
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x5D() {
    // ld e, ixl
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x5E() {
    // ld e, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    ldReg8(gpRegisters[cpuReg::DE].lo, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x5F() {
    // ld e, a
    ldReg8(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x60() {
    // ld ixh, b
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x61() {
    // ld ixh, c
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x62() {
    // ld ixh, d
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x63() {
    // ld ixh, e
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x64() {
    // ld ixh, ixh
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x65() {
    // ld ixh, ixl
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x66() {
    // ld h, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    ldReg8(gpRegisters[cpuReg::HL].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x67() {
    // ld ixh, a
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x68() {
    // ld ixl, b
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].lo, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x69() {
    // ld ixl, c
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x6A() {
    // ld ixl, d
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].lo, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x6B() {
    // ld ixl, e
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].lo, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x6C() {
    // ld ixl, ixh
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].lo, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x6D() {
    // ld ixl, ixl
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].lo, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x6E() {
    // ld l, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    ldReg8(gpRegisters[cpuReg::HL].lo, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x6F() {
    // ld ixl, a
    ldReg8(gpRegisters[indexRegisterForCurrentOpcode].lo, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x70() {
    // ld (ix+d), b
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, gpRegisters[cpuReg::BC].hi);
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x71() {
    // ld (ix+d), c
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, gpRegisters[cpuReg::BC].lo);
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x72() {
    // ld (ix+d), d
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, gpRegisters[cpuReg::DE].hi);
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x73() {
    // ld (ix+d), e
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, gpRegisters[cpuReg::DE].lo);
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x74() {
    // ld (ix+d), h
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, gpRegisters[cpuReg::HL].hi);
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x75() {
    // ld (ix+d), l
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, gpRegisters[cpuReg::HL].lo);
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x77() {
    // ld (ix+d), a
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    writeMemory(address, gpRegisters[cpuReg::AF].hi);
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x78() {
    // ld a, b
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x79() {
    // ld a, c
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x7A() {
    // ld a, d
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x7B() {
    // ld a, e
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x7C() {
    // ld a, ixh
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x7D() {
    // ld a, ixl
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x7E() {
    // ld a, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    ldReg8(gpRegisters[cpuReg::AF].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x7F() {
    // ld a, a
    ldReg8(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x80() {
    // add a, b
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x81() {
    // add a, c
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x82() {
    // add a, d
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x83() {
    // add a, e
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x84() {
    // add a, ixh
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x85() {
    // add a, ixl
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x86() {
    // add a, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    add8Bit(gpRegisters[cpuReg::AF].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x87() {
    // add a, a
    add8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x88() {
    // adc a, b
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x89() {
    // adc a, c
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x8A() {
    // adc a, d
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x8B() {
    // adc a, e
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x8C() {
    // adc a, ixh
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x8D() {
    // adc a, ixl
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x8E() {
    // adc a, (ix+d)
    adc8Bit(gpRegisters[cpuReg::AF].hi, readMemory(getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole)));
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x8F() {
    // adc a, a
    adc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x90() {
    // sub a, b
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x91() {
    // sub a, c
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x92() {
    // sub a, d
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x93() {
    // sub a, e
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x94() {
    // sub a, ixh
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x95() {
    // sub a, ixl
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x96() {
    // sub a, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    sub8Bit(gpRegisters[cpuReg::AF].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x97() {
    // sub a, a
    sub8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x98() {
    // sbc a, b
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x99() {
    // sbc a, c
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x9A() {
    // sbc a, d
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x9B() {
    // sbc a, e
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x9C() {
    // sbc a, ixh
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x9D() {
    // sbc a, ixl
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0x9E() {
    // sbc a, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    sbc8Bit(gpRegisters[cpuReg::AF].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0x9F() {
    // sbc a, a
    sbc8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA0() {
    // and b
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA1() {
    // and c
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA2() {
    // and d
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA3() {
    // and e
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA4() {
    // and ixh
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA5() {
    // and ixl
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA6() {
    // and a, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    and8Bit(gpRegisters[cpuReg::AF].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0xA7() {
    // and a
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA8() {
    // xor b
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xA9() {
    // xor c
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xAA() {
    // xor d
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xAB() {
    // xor e
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xAC() {
    // xor ixh
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xAD() {
    // xor ixl
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xAE() {
    // xor a, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    exclusiveOr(gpRegisters[cpuReg::AF].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0xAF() {
    // xor a
    exclusiveOr(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB0() {
    // or b
    and8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB1() {
    // or c
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB2() {
    // or d
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB3() {
    // or e
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB4() {
    // or ixh
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB5() {
    // or ixl
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB6() {
    // or a, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    or8Bit(gpRegisters[cpuReg::AF].hi, readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0xB7() {
    // or a
    or8Bit(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB8() {
    // cp b
    compare8Bit(gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xB9() {
    // cp c
    compare8Bit(gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xBA() {
    // cp d
    compare8Bit(gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xBB() {
    // cp e
    compare8Bit(gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xBC() {
    // cp ixh
    compare8Bit(gpRegisters[indexRegisterForCurrentOpcode].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xBD() {
    // cp ixl
    compare8Bit(gpRegisters[indexRegisterForCurrentOpcode].lo);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xBE() {
    // cp a, (ix+d)
    unsigned short address = getIndexedOffsetAddress(gpRegisters[indexRegisterForCurrentOpcode].whole);
    compare8Bit(readMemory(address));
    gpRegisters[cpuReg::WZ].whole = address;
    cyclesTaken = 19;
}

void CPUZ80::indexOpcodeHandler0xBF() {
    // cp a
    compare8Bit(gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xCB() {
    indexBitOpcodes(indexRegisterForCurrentOpcode);
}

void CPUZ80::indexOpcodeHandler0xE1() {
    // pop ix
    ldReg16(gpRegisters[indexRegisterForCurrentOpcode].whole, popStack16(), false);
    cyclesTaken = 14;
}

void CPUZ80::indexOpcodeHandler0xE3() {
    // ex (sp), ix
    exStack(gpRegisters[indexRegisterForCurrentOpcode].whole);
    cyclesTaken = 23;
}

void CPUZ80::indexOpcodeHandler0xE5() {
    // push ix
    pushStack(gpRegisters[indexRegisterForCurrentOpcode].whole);
    cyclesTaken = 15;
}

void CPUZ80::indexOpcodeHandler0xE9() {
    // jp (ix) - Notation seems to indicate that this is indirect, but docs says that it is
    programCounter = gpRegisters[indexRegisterForCurrentOpcode].whole;
    cyclesTaken = 8;
}

void CPUZ80::indexOpcodeHandler0xF9() {
    ldReg16(stackPointer, gpRegisters[indexRegisterForCurrentOpcode].whole);
    cyclesTaken = 10;
}