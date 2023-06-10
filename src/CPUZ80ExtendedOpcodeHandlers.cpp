//
// Created by Peter Savory on 09/06/2023.
//

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"
#include <Exceptions.h>

void CPUZ80::extendedOpcodeHandler0x40() {
    // in b, (c)
    readPortToRegister(gpRegisters[cpuReg::BC].hi, gpRegisters[cpuReg::BC].lo);
}

void CPUZ80::extendedOpcodeHandler0x41() {
    // out (c), b
    portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x42() {
    // sbc hl, bc
    sbc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].whole);
    cyclesTaken = 15;
}

void CPUZ80::extendedOpcodeHandler0x43() {
    // ld (nn), bc
    writeMemory(build16BitNumber(), gpRegisters[cpuReg::BC].whole);
    cyclesTaken = 20;
}

void CPUZ80::extendedOpcodeHandler0x44() {
    // neg
    neg();
    cyclesTaken = 8;
}

void CPUZ80::extendedOpcodeHandler0x45() {
    // retn
    retn();
}

void CPUZ80::extendedOpcodeHandler0x46() {
    // im 0
    setInterruptMode(0);
}

void CPUZ80::extendedOpcodeHandler0x47() {
    // ld i, a
    ldReg8(registerI, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 9;
}

void CPUZ80::extendedOpcodeHandler0x48() {
    // in c, (c)
    readPortToRegister(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].lo);
}

void CPUZ80::extendedOpcodeHandler0x49() {
    // out (c), c
    portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x4A() {
    // adc hl, bc
    adc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::BC].whole);
    cyclesTaken = 15;
}

void CPUZ80::extendedOpcodeHandler0x4B() {
    // ld bc, (nn)
    ldReg16(gpRegisters[cpuReg::BC].whole, readMemory16Bit(build16BitNumber()), false);
    cyclesTaken = 20;
}

void CPUZ80::extendedOpcodeHandler0x4D() {
    // reti
    reti();
}

void CPUZ80::extendedOpcodeHandler0x4F() {
    // ld r, a
    ldReg8(registerR, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 9;
}

void CPUZ80::extendedOpcodeHandler0x50() {
    // in d, (c)
    readPortToRegister(gpRegisters[cpuReg::DE].hi, gpRegisters[cpuReg::BC].lo);
}

void CPUZ80::extendedOpcodeHandler0x51() {
    // out (c), d
    portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x52() {
    // sbc hl, de
    sbc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].whole);
    cyclesTaken = 15;
}

void CPUZ80::extendedOpcodeHandler0x53() {
    // ld (nn), de
    writeMemory(build16BitNumber(), gpRegisters[cpuReg::DE].whole);
    cyclesTaken = 20;
}

void CPUZ80::extendedOpcodeHandler0x56() {
    // im 1
    setInterruptMode(1);
}

void CPUZ80::extendedOpcodeHandler0x57() {
    // ld a, i
    ldReg8(gpRegisters[cpuReg::AF].hi, registerI, true);
    cyclesTaken = 9;
}

void CPUZ80::extendedOpcodeHandler0x58() {
    // in e, (c)
    readPortToRegister(gpRegisters[cpuReg::DE].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x59() {
    // out (c), e
    portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x5A() {
    // adc hl, de
    adc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::DE].whole);
    cyclesTaken = 15;
}

void CPUZ80::extendedOpcodeHandler0x5B() {
    // ld de, (nn)
    ldReg16(gpRegisters[cpuReg::DE].whole, readMemory16Bit(build16BitNumber()));
    cyclesTaken = 20;
}

void CPUZ80::extendedOpcodeHandler0x5E() {
    setInterruptMode(2);
}

void CPUZ80::extendedOpcodeHandler0x5F() {
    // ld a, r
    ldReg8(gpRegisters[cpuReg::AF].hi, registerR, true);
    cyclesTaken = 9;
}

void CPUZ80::extendedOpcodeHandler0x60() {
    // in h, (c)
    readPortToRegister(gpRegisters[cpuReg::HL].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x61() {
    // out (c), h
    portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x62() {
    // sbc hl, hl
    sbc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].whole);
    cyclesTaken = 15;
}

void CPUZ80::extendedOpcodeHandler0x63() {
    // ld (nn), hl
    writeMemory(build16BitNumber(), gpRegisters[cpuReg::HL].whole);
    cyclesTaken = 20;
}

void CPUZ80::extendedOpcodeHandler0x67() {
    // rrd
    rrd(gpRegisters[cpuReg::AF].hi);
}

void CPUZ80::extendedOpcodeHandler0x68() {
    // in l, (c)
    readPortToRegister(gpRegisters[cpuReg::HL].lo, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x69() {
    // out (c), l
    portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x6A() {
    // adc hl, hl
    adc16Bit(gpRegisters[cpuReg::HL].whole, gpRegisters[cpuReg::HL].whole);
    cyclesTaken = 15;
}

void CPUZ80::extendedOpcodeHandler0x6B() {
    // ld hl, (nn)
    ldReg16(gpRegisters[cpuReg::HL].whole, readMemory16Bit(build16BitNumber()));
    cyclesTaken = 20;
}

void CPUZ80::extendedOpcodeHandler0x6F() {
    // rld
    rld(gpRegisters[cpuReg::AF].hi);
}

void CPUZ80::extendedOpcodeHandler0x70() {
    // in (c)
    z80Io->read(gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x71() {
    // out (c), 0
    portOut(gpRegisters[cpuReg::BC].lo, 0);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x72() {
    // sbc hl, sp
    sbc16Bit(gpRegisters[cpuReg::HL].whole, stackPointer);
    cyclesTaken = 15;
}

void CPUZ80::extendedOpcodeHandler0x73() {
    // ld (nn), sp
    writeMemory(build16BitNumber(), stackPointer);
    cyclesTaken = 20;
}

void CPUZ80::extendedOpcodeHandler0x78() {
    // in a, (c)
    readPortToRegister(gpRegisters[cpuReg::AF].hi, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x79() {
    // out (c), a
    portOut(gpRegisters[cpuReg::BC].lo, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 12;
}

void CPUZ80::extendedOpcodeHandler0x7A() {
    // adc hl, sp
    adc16Bit(gpRegisters[cpuReg::HL].whole, stackPointer);
    cyclesTaken = 15;
}

void CPUZ80::extendedOpcodeHandler0x7B() {
    // ld sp, (nn)
    ldReg16(stackPointer, readMemory16Bit(build16BitNumber()));
    cyclesTaken = 20;
}

void CPUZ80::extendedOpcodeHandler0xA0() {
    // ldi
    ldi(true);
}

void CPUZ80::extendedOpcodeHandler0xA1() {
    // cpi
    cpi(true);
}

void CPUZ80::extendedOpcodeHandler0xA2() {
    // ini
    ini(true);
}

void CPUZ80::extendedOpcodeHandler0xA3() {
    // outi
    outi(true);
}

void CPUZ80::extendedOpcodeHandler0xA8() {
    // ldd
    ldi(false);
}

void CPUZ80::extendedOpcodeHandler0xA9() {
    // cpd
    cpi(false);
}

void CPUZ80::extendedOpcodeHandler0xAA() {
    ini(false);
}

void CPUZ80::extendedOpcodeHandler0xAB() {
    outi(false);
}

void CPUZ80::extendedOpcodeHandler0xB0() {
    // ldir
    ldir(true);
}

void CPUZ80::extendedOpcodeHandler0xB1() {
    // cpir
    cpir(true);
}

void CPUZ80::extendedOpcodeHandler0xB2() {
    // inir
    inir(true);
}

void CPUZ80::extendedOpcodeHandler0xB3() {
    // otir
    otir(true);
}

void CPUZ80::extendedOpcodeHandler0xB8() {
    // lddr
    ldir(false);
}

void CPUZ80::extendedOpcodeHandler0xB9() {
    // cpdr
    cpir(false);
}

void CPUZ80::extendedOpcodeHandler0xBA() {
    // indr
    inir(false);
}

void CPUZ80::extendedOpcodeHandler0xBB() {
    // otdr
    otir(false);
}