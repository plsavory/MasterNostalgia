//
// Created by Peter Savory on 09/06/2023.
//

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"
#include <Exceptions.h>

void CPUZ80::bitOpcodeHandler0x00() {
    // rlc b
    ldReg8(gpRegisters[cpuReg::BC].hi, rlc(gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x01() {
    // rlc c
    ldReg8(gpRegisters[cpuReg::BC].lo, rlc(gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x02() {
    // rlc d
    ldReg8(gpRegisters[cpuReg::DE].hi, rlc(gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x03() {
    // rlc e
    ldReg8(gpRegisters[cpuReg::DE].lo, rlc(gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x04() {
    // rlc h
    ldReg8(gpRegisters[cpuReg::HL].hi, rlc(gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x05() {
    // rlc l
    ldReg8(gpRegisters[cpuReg::HL].lo, rlc(gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x06() {
    // rlc (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, rlc(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x07() {
    // rlc a
    ldReg8(gpRegisters[cpuReg::AF].hi, rlc(gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x08() {
    // rrc b
    ldReg8(gpRegisters[cpuReg::BC].hi, rrc(gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x09() {
    // rrc c
    ldReg8(gpRegisters[cpuReg::BC].lo, rrc(gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x0A() {
    // rrc d
    ldReg8(gpRegisters[cpuReg::DE].hi, rrc(gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x0B() {
    // rrc e
    ldReg8(gpRegisters[cpuReg::DE].lo, rrc(gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x0C() {
    // rrc h
    ldReg8(gpRegisters[cpuReg::HL].hi, rrc(gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x0D() {
    // rrc l
    ldReg8(gpRegisters[cpuReg::HL].lo, rrc(gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x0E() {
    // rrc (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, rrc(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x0F() {
    // rrc a
    ldReg8(gpRegisters[cpuReg::AF].hi, rrc(gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x10() {
    // rl b
    ldReg8(gpRegisters[cpuReg::BC].hi, rl(gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x11() {
    // rl c
    ldReg8(gpRegisters[cpuReg::BC].lo, rl(gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x12() {
    // rl d
    ldReg8(gpRegisters[cpuReg::DE].hi, rl(gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x13() {
    // rl e
    ldReg8(gpRegisters[cpuReg::DE].lo, rl(gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x14() {
    // rl h
    ldReg8(gpRegisters[cpuReg::HL].hi, rl(gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x15() {
    // rl l
    ldReg8(gpRegisters[cpuReg::HL].lo, rl(gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x16() {
    // rl (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, rl(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x17() {
    // rl a
    ldReg8(gpRegisters[cpuReg::AF].hi, rl(gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x18() {
    // rr b
    ldReg8(gpRegisters[cpuReg::BC].hi, rr(gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x19() {
    // rr c
    ldReg8(gpRegisters[cpuReg::BC].lo, rr(gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x1A() {
    // rr d
    ldReg8(gpRegisters[cpuReg::DE].hi, rr(gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x1B() {
    // rr e
    ldReg8(gpRegisters[cpuReg::DE].lo, rr(gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x1C() {
    // rr h
    ldReg8(gpRegisters[cpuReg::HL].hi, rr(gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x1D() {
    // rr l
    ldReg8(gpRegisters[cpuReg::HL].lo, rr(gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x1E() {
    // rr (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, rr(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x1F() {
    // rr a
    ldReg8(gpRegisters[cpuReg::AF].hi, rr(gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x20() {
    // sla b
    ldReg8(gpRegisters[cpuReg::BC].hi, sla(gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x21() {
    // sla c
    ldReg8(gpRegisters[cpuReg::BC].lo, sla(gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x22() {
    // sla d
    ldReg8(gpRegisters[cpuReg::DE].hi, sla(gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x23() {
    // sla e
    ldReg8(gpRegisters[cpuReg::DE].lo, sla(gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x24() {
    // sla h
    ldReg8(gpRegisters[cpuReg::HL].hi, sla(gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x25() {
    // sla l
    ldReg8(gpRegisters[cpuReg::HL].lo, sla(gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x26() {
    // sla (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, sla(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x27() {
    // sla a
    ldReg8(gpRegisters[cpuReg::AF].hi, sla(gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x28() {
    // sra b
    ldReg8(gpRegisters[cpuReg::BC].hi, sra(gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x29() {
    // sra c
    ldReg8(gpRegisters[cpuReg::BC].lo, sra(gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x2A() {
    // sra d
    ldReg8(gpRegisters[cpuReg::DE].hi, sra(gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x2B() {
    // sra e
    ldReg8(gpRegisters[cpuReg::DE].lo, sra(gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x2C() {
    // sra h
    ldReg8(gpRegisters[cpuReg::HL].hi, sra(gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x2D() {
    // sra l
    ldReg8(gpRegisters[cpuReg::HL].lo, sra(gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x2E() {
    // sra (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, sra(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x2F() {
    // sra a
    ldReg8(gpRegisters[cpuReg::AF].hi, sra(gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x30() {
    // sll b
    ldReg8(gpRegisters[cpuReg::BC].hi, sll(gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x31() {
    // sll c
    ldReg8(gpRegisters[cpuReg::BC].lo, sll(gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x32() {
    // sll d
    ldReg8(gpRegisters[cpuReg::DE].hi, sll(gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x33() {
    // sll e
    ldReg8(gpRegisters[cpuReg::DE].lo, sll(gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x34() {
    // sll h
    ldReg8(gpRegisters[cpuReg::HL].hi, sll(gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x35() {
    // sll l
    ldReg8(gpRegisters[cpuReg::HL].lo, sll(gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x36() {
    // sll (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, sll(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x37() {
    // sll a
    ldReg8(gpRegisters[cpuReg::AF].hi, sll(gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x38() {
    // srl b
    ldReg8(gpRegisters[cpuReg::BC].hi, srl(gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x39() {
    // srl c
    ldReg8(gpRegisters[cpuReg::BC].lo, srl(gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x3A() {
    // srl d
    ldReg8(gpRegisters[cpuReg::DE].hi, srl(gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x3B() {
    // srl e
    ldReg8(gpRegisters[cpuReg::DE].lo, srl(gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x3C() {
    // srl h
    ldReg8(gpRegisters[cpuReg::HL].hi, srl(gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x3D() {
    // srl l
    ldReg8(gpRegisters[cpuReg::HL].lo, srl(gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x3E() {
    // srl (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, srl(readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x3F() {
    // srl a
    ldReg8(gpRegisters[cpuReg::AF].hi, srl(gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x40() {
    // bit 0, b
    bit(0, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x41() {
    // bit 0, c
    bit(0, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x42() {
    // bit 0, d
    bit(0, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x43() {
    // bit 0, e
    bit(0, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x44() {
    // bit 0, h
    bit(0, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x45() {
    // bit 0, l
    bit(0, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x46() {
    // bit 0, (hl);
    hlBit(0);
    cyclesTaken = 12;
}

void CPUZ80::bitOpcodeHandler0x47() {
    // bit 0, a
    bit(0, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x48() {
    // bit 1, b
    bit(1, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x49() {
    // bit 1, c
    bit(1, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x4A() {
    // bit 1, d
    bit(1, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x4B() {
    // bit 1, e
    bit(1, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x4C() {
    // bit 1, h
    bit(1, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x4D() {
    // bit 1, l
    bit(1, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x4E() {
    // bit 1, (hl);
    hlBit(1);
    cyclesTaken = 12;
}

void CPUZ80::bitOpcodeHandler0x4F() {
    // bit 1, a
    bit(1, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x50() {
    // bit 2, b
    bit(2, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x51() {
    // bit 2, c
    bit(2, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x52() {
    // bit 2, d
    bit(2, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x53() {
    // bit 2, e
    bit(2, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x54() {
    // bit 2, h
    bit(2, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x55() {
    // bit 2, l
    bit(2, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x56() {
    // bit 2, (hl);
    hlBit(2);
    cyclesTaken = 12;
}

void CPUZ80::bitOpcodeHandler0x57() {
    // bit 2, a
    bit(2, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x58() {
    // bit 3, b
    bit(3, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x59() {
    // bit 3, c
    bit(3, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x5A() {
    // bit 3, d
    bit(3, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x5B() {
    // bit 3, e
    bit(3, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x5C() {
    // bit 3, h
    bit(3, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x5D() {
    // bit 3, l
    bit(3, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x5E() {
    // bit 3, (hl);
    hlBit(3);
    cyclesTaken = 12;
}

void CPUZ80::bitOpcodeHandler0x5F() {
    // bit 3, a
    bit(3, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x60() {
    // bit 4, b
    bit(4, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x61() {
    // bit 4, c
    bit(4, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x62() {
    // bit 4, d
    bit(4, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x63() {
    // bit 4, e
    bit(4, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x64() {
    // bit 4, h
    bit(4, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x65() {
    // bit 4, l
    bit(4, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x66() {
    // bit 4, (hl);
    hlBit(4);
    cyclesTaken = 12;
}

void CPUZ80::bitOpcodeHandler0x67() {
    // bit 4, a
    bit(4, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x68() {
    // bit 5, b
    bit(5, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x69() {
    // bit 5, c
    bit(5, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x6A() {
    // bit 5, d
    bit(5, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x6B() {
    // bit 5, e
    bit(5, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x6C() {
    // bit 5, h
    bit(5, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x6D() {
    // bit 5, l
    bit(5, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x6E() {
    // bit 5, (hl);
    hlBit(5);
    cyclesTaken = 12;
}

void CPUZ80::bitOpcodeHandler0x6F() {
    // bit 5, a
    bit(5, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x70() {
    // bit 6, b
    bit(6, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x71() {
    // bit 6, c
    bit(6, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x72() {
    // bit 6, d
    bit(6, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x73() {
    // bit 6, e
    bit(6, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x74() {
    // bit 6, h
    bit(6, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x75() {
    // bit 6, l
    bit(6, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x76() {
    // bit 6, (hl);
    hlBit(6);
    cyclesTaken = 12;
}

void CPUZ80::bitOpcodeHandler0x77() {
    // bit 6, a
    bit(6, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x78() {
    // bit 7, b
    bit(7, gpRegisters[cpuReg::BC].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x79() {
    // bit 7, c
    bit(7, gpRegisters[cpuReg::BC].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x7A() {
    // bit 7, d
    bit(7, gpRegisters[cpuReg::DE].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x7B() {
    // bit 7, e
    bit(7, gpRegisters[cpuReg::DE].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x7C() {
    // bit 7, h
    bit(7, gpRegisters[cpuReg::HL].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x7D() {
    // bit 7, l
    bit(7, gpRegisters[cpuReg::HL].lo);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x7E() {
    // bit 7, (hl);
    hlBit(7);
    cyclesTaken = 12;
}

void CPUZ80::bitOpcodeHandler0x7F() {
    // bit 7, a
    bit(7, gpRegisters[cpuReg::AF].hi);
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x80() {
    // res 0, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(0, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x81() {
    // res 0, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(0, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x82() {
    // res 0, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(0, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x83() {
    // res 0, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(0, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x84() {
    // res 0, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(0, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x85() {
    // res 0, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(0, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x86() {
    // res 0, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, res(0, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x87() {
    // res 0, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(0, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x88() {
    // res 1, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(1, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x89() {
    // res 1, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(1, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x8A() {
    // res 1, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(1, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x8B() {
    // res 1, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(1, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x8C() {
    // res 1, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(1, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x8D() {
    // res 1, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(1, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x8E() {
    // res 1, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, res(1, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x8F() {
    // res 1, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(1, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x90() {
    // res 2, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(2, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x91() {
    // res 2, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(2, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x92() {
    // res 2, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(2, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x93() {
    // res 2, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(2, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x94() {
    // res 2, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(2, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x95() {
    // res 2, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(2, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x96() {
    // res 2, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, res(2, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x97() {
    // res 2, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(2, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x98() {
    // res 3, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(3, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x99() {
    // res 3, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(3, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x9A() {
    // res 3, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(3, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x9B() {
    // res 3, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(3, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x9C() {
    // res 3, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(3, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x9D() {
    // res 3, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(3, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0x9E() {
    // res 3, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, res(3, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0x9F() {
    // res 3, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(3, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA0() {
    // res 4, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(4, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA1() {
    // res 4, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(4, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA2() {
    // res 4, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(4, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA3() {
    // res 4, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(4, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA4() {
    // res 4, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(4, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA5() {
    // res 4, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(4, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA6() {
    // res 4, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, res(4, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xA7() {
    // res 4, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(4, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA8() {
    // res 5, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(5, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xA9() {
    // res 5, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(5, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xAA() {
    // res 5, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(5, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xAB() {
    // res 5, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(5, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xAC() {
    // res 5, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(5, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xAD() {
    // res 5, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(5, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xAE() {
    // res 5, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, res(5, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xAF() {
    // res 5, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(5, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB0() {
    // res 6, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(6, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB1() {
    // res 6, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(6, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB2() {
    // res 6, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(6, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB3() {
    // res 6, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(6, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB4() {
    // res 6, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(6, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB5() {
    // res 6, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(6, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB6() {
    // res 6, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, res(6, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xB7() {
    // res 6, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(6, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB8() {
    // res 7, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(7, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xB9() {
    // res 7, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(7, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xBA() {
    // res 7, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(7, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xBB() {
    // res 7, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(7, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xBC() {
    // res 7, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(7, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xBD() {
    // res 7, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(7, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xBE() {
    // res 7, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, res(7, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xBF() {
    // res 7, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(7, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC0() {
    // set 0, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(0, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC1() {
    // set 0, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(0, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC2() {
    // set 0, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(0, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC3() {
    // set 0, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(0, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC4() {
    // set 0, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(0, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC5() {
    // set 0, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(0, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC6() {
    // set 0, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, set(0, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xC7() {
    // set 0, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(0, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC8() {
    // set 1, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(1, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xC9() {
    // set 1, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(1, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xCA() {
    // set 1, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(1, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xCB() {
    // set 1, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(1, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xCC() {
    // set 1, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(1, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xCD() {
    // set 1, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(1, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xCE() {
    // set 1, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, set(1, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xCF() {
    // set 1, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(1, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD0() {
    // set 2, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(2, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD1() {
    // set 2, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(2, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD2() {
    // set 2, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(2, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD3() {
    // set 2, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(2, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD4() {
    // set 2, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(2, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD5() {
    // set 2, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(2, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD6() {
    // set 2, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, set(2, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xD7() {
    // set 2, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(2, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD8() {
    // set 3, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(3, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xD9() {
    // set 3, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(3, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xDA() {
    // set 3, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(3, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xDB() {
    // set 3, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(3, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xDC() {
    // set 3, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(3, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xDD() {
    // set 3, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(3, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xDE() {
    // set 3, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, set(3, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xDF() {
    // set 3, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(3, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE0() {
    // set 4, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(4, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE1() {
    // set 4, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(4, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE2() {
    // set 4, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(4, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE3() {
    // set 4, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(4, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE4() {
    // set 4, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(4, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE5() {
    // set 4, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(4, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE6() {
    // set 4, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, set(4, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xE7() {
    // set 4, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(4, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE8() {
    // set 5, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(5, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xE9() {
    // set 5, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(5, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xEA() {
    // set 5, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(5, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xEB() {
    // set 5, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(5, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xEC() {
    // set 5, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(5, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xED() {
    // set 5, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(5, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xEE() {
    // set 5, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, set(5, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xEF() {
    // set 5, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(5, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF0() {
    // set 6, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(6, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF1() {
    // set 6, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(6, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF2() {
    // set 6, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(6, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF3() {
    // set 6, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(6, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF4() {
    // set 6, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(6, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF5() {
    // set 6, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(6, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF6() {
    // set 6, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, set(6, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xF7() {
    // set 6, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(6, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF8() {
    // set 7, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(7, gpRegisters[cpuReg::BC].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xF9() {
    // set 7, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(7, gpRegisters[cpuReg::BC].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xFA() {
    // set 7, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(7, gpRegisters[cpuReg::DE].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xFB() {
    // set 7, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(7, gpRegisters[cpuReg::DE].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xFC() {
    // set 7, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(7, gpRegisters[cpuReg::HL].hi));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xFD() {
    // set 7, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(7, gpRegisters[cpuReg::HL].lo));
    cyclesTaken = 8;
}

void CPUZ80::bitOpcodeHandler0xFE() {
    // set 7, (hl)
    writeMemory(gpRegisters[cpuReg::HL].whole, set(7, readMemory(gpRegisters[cpuReg::HL].whole)));
    cyclesTaken = 15;
}

void CPUZ80::bitOpcodeHandler0xFF() {
    // set 7, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(7, gpRegisters[cpuReg::AF].hi));
    cyclesTaken = 8;
}