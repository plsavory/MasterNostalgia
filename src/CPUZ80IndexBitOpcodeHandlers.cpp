//
// Created by Peter Savory on 09/06/2023.
//

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"
#include <Exceptions.h>

void CPUZ80::indexBitOpcodeHandler0x00() {
    // rlc b
    ldReg8(gpRegisters[cpuReg::BC].hi, rlc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x01() {
    // rlc c
    ldReg8(gpRegisters[cpuReg::BC].lo, rlc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x02() {
    // rlc d
    ldReg8(gpRegisters[cpuReg::DE].hi, rlc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x03() {
    // rlc e
    ldReg8(gpRegisters[cpuReg::DE].lo, rlc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x04() {
    // rlc h
    ldReg8(gpRegisters[cpuReg::HL].hi, rlc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x05() {
    // rlc l
    ldReg8(gpRegisters[cpuReg::HL].lo, rlc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x06() {
    // rlc (hl)
    writeMemory(indexedAddressForCurrentOpcode, rlc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x07() {
    // rlc a
    ldReg8(gpRegisters[cpuReg::AF].hi, rlc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x08() {
    // rrc b
    ldReg8(gpRegisters[cpuReg::BC].hi, rrc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x09() {
    // rrc c
    ldReg8(gpRegisters[cpuReg::BC].lo, rrc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x0A() {
    // rrc d
    ldReg8(gpRegisters[cpuReg::DE].hi, rrc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x0B() {
    // rrc e
    ldReg8(gpRegisters[cpuReg::DE].lo, rrc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x0C() {
    // rrc h
    ldReg8(gpRegisters[cpuReg::HL].hi, rrc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x0D() {
    // rrc l
    ldReg8(gpRegisters[cpuReg::HL].lo, rrc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x0E() {
    // rrc (hl)
    writeMemory(indexedAddressForCurrentOpcode, rrc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x0F() {
    // rrc a
    ldReg8(gpRegisters[cpuReg::AF].hi, rrc(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x10() {
    // rl b
    ldReg8(gpRegisters[cpuReg::BC].hi, rl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x11() {
    // rl c
    ldReg8(gpRegisters[cpuReg::BC].lo, rl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x12() {
    // rl d
    ldReg8(gpRegisters[cpuReg::DE].hi, rl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x13() {
    // rl e
    ldReg8(gpRegisters[cpuReg::DE].lo, rl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x14() {
    // rl h
    ldReg8(gpRegisters[cpuReg::HL].hi, rl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x15() {
    // rl l
    ldReg8(gpRegisters[cpuReg::HL].lo, rl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x16() {
    // rl (hl)
    writeMemory(indexedAddressForCurrentOpcode, rl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x17() {
    // rl a
    ldReg8(gpRegisters[cpuReg::AF].hi, rl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x18() {
    // rr b
    ldReg8(gpRegisters[cpuReg::BC].hi, rr(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x19() {
    // rr c
    ldReg8(gpRegisters[cpuReg::BC].lo, rr(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x1A() {
    // rr d
    ldReg8(gpRegisters[cpuReg::DE].hi, rr(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x1B() {
    // rr e
    ldReg8(gpRegisters[cpuReg::DE].lo, rr(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x1C() {
    // rr h
    ldReg8(gpRegisters[cpuReg::HL].hi, rr(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x1D() {
    // rr l
    ldReg8(gpRegisters[cpuReg::HL].lo, rr(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x1E() {
    // rr (hl)
    writeMemory(indexedAddressForCurrentOpcode, rr(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x1F() {
    // rr a
    ldReg8(gpRegisters[cpuReg::AF].hi, rr(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x20() {
    // sla b
    ldReg8(gpRegisters[cpuReg::BC].hi, sla(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x21() {
    // sla c
    ldReg8(gpRegisters[cpuReg::BC].lo, sla(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x22() {
    // sla d
    ldReg8(gpRegisters[cpuReg::DE].hi, sla(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x23() {
    // sla e
    ldReg8(gpRegisters[cpuReg::DE].lo, sla(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x24() {
    // sla h
    ldReg8(gpRegisters[cpuReg::HL].hi, sla(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x25() {
    // sla l
    ldReg8(gpRegisters[cpuReg::HL].lo, sla(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x26() {
    // sla (hl)
    writeMemory(indexedAddressForCurrentOpcode, sla(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x27() {
    // sla a
    ldReg8(gpRegisters[cpuReg::AF].hi, sla(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x28() {
    // sra b
    ldReg8(gpRegisters[cpuReg::BC].hi, sra(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x29() {
    // sra c
    ldReg8(gpRegisters[cpuReg::BC].lo, sra(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x2A() {
    // sra d
    ldReg8(gpRegisters[cpuReg::DE].hi, sra(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x2B() {
    // sra e
    ldReg8(gpRegisters[cpuReg::DE].lo, sra(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x2C() {
    // sra h
    ldReg8(gpRegisters[cpuReg::HL].hi, sra(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x2D() {
    // sra l
    ldReg8(gpRegisters[cpuReg::HL].lo, sra(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x2E() {
    // sra (hl)
    writeMemory(indexedAddressForCurrentOpcode, sra(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x2F() {
    // sra a
    ldReg8(gpRegisters[cpuReg::AF].hi, sra(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x30() {
    // sll b
    ldReg8(gpRegisters[cpuReg::BC].hi, sll(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x31() {
    // sll c
    ldReg8(gpRegisters[cpuReg::BC].lo, sll(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x32() {
    // sll d
    ldReg8(gpRegisters[cpuReg::DE].hi, sll(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x33() {
    // sll e
    ldReg8(gpRegisters[cpuReg::DE].lo, sll(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x34() {
    // sll h
    ldReg8(gpRegisters[cpuReg::HL].hi, sll(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x35() {
    // sll l
    ldReg8(gpRegisters[cpuReg::HL].lo, sll(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x36() {
    // sll (hl)
    writeMemory(indexedAddressForCurrentOpcode, sll(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x37() {
    // sll a
    ldReg8(gpRegisters[cpuReg::AF].hi, sll(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x38() {
    // srl b
    ldReg8(gpRegisters[cpuReg::BC].hi, srl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x39() {
    // srl c
    ldReg8(gpRegisters[cpuReg::BC].lo, srl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x3A() {
    // srl d
    ldReg8(gpRegisters[cpuReg::DE].hi, srl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x3B() {
    // srl e
    ldReg8(gpRegisters[cpuReg::DE].lo, srl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x3C() {
    // srl h
    ldReg8(gpRegisters[cpuReg::HL].hi, srl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x3D() {
    // srl l
    ldReg8(gpRegisters[cpuReg::HL].lo, srl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x3E() {
    // srl (hl)
    writeMemory(indexedAddressForCurrentOpcode, srl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x3F() {
    // srl a
    ldReg8(gpRegisters[cpuReg::AF].hi, srl(readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x40() {
    // bit 0, b
    bit(0, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x41() {
    // bit 0, c
    bit(0, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x42() {
    // bit 0, d
    bit(0, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x43() {
    // bit 0, e
    bit(0, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x44() {
    // bit 0, h
    bit(0, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x45() {
    // bit 0, l
    bit(0, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x46() {
    // bit 0, (hl);
    bit(0, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x47() {
    // bit 0, a
    bit(0, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x48() {
    // bit 1, b
    bit(1, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x49() {
    // bit 1, c
    bit(1, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x4A() {
    // bit 1, d
    bit(1, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x4B() {
    // bit 1, e
    bit(1, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x4C() {
    // bit 1, h
    bit(1, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x4D() {
    // bit 1, l
    bit(1, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x4E() {
    // bit 1, (hl);
    bit(1, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x4F() {
    // bit 1, a
    bit(1, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x50() {
    // bit 2, b
    bit(2, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x51() {
    // bit 2, c
    bit(2, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x52() {
    // bit 2, d
    bit(2, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x53() {
    // bit 2, e
    bit(2, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x54() {
    // bit 2, h
    bit(2, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x55() {
    // bit 2, l
    bit(2, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x56() {
    // bit 2, (hl);
    bit(2, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x57() {
    // bit 2, a
    bit(2, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x58() {
    // bit 3, b
    bit(3, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x59() {
    // bit 3, c
    bit(3, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x5A() {
    // bit 3, d
    bit(3, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x5B() {
    // bit 3, e
    bit(3, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x5C() {
    // bit 3, h
    bit(3, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x5D() {
    // bit 3, l
    bit(3, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x5E() {
    // bit 3, (hl);
    bit(3, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x5F() {
    // bit 3, a
    bit(3, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x60() {
    // bit 4, b
    bit(4, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x61() {
    // bit 4, c
    bit(4, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x62() {
    // bit 4, d
    bit(4, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x63() {
    // bit 4, e
    bit(4, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x64() {
    // bit 4, h
    bit(4, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x65() {
    // bit 4, l
    bit(4, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x66() {
    // bit 4, (hl);
    bit(4, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x67() {
    // bit 4, a
    bit(4, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x68() {
    // bit 5, b
    bit(5, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x69() {
    // bit 5, c
    bit(5, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x6A() {
    // bit 5, d
    bit(5, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x6B() {
    // bit 5, e
    bit(5, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x6C() {
    // bit 5, h
    bit(5, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x6D() {
    // bit 5, l
    bit(5, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x6E() {
    // bit 5, (hl);
    bit(5, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x6F() {
    // bit 5, a
    bit(5, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x70() {
    // bit 6, b
    bit(6, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x71() {
    // bit 6, c
    bit(6, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x72() {
    // bit 6, d
    bit(6, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x73() {
    // bit 6, e
    bit(6, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x74() {
    // bit 6, h
    bit(6, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x75() {
    // bit 6, l
    bit(6, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x76() {
    // bit 6, (hl);
    bit(6, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x77() {
    // bit 6, a
    bit(6, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x78() {
    // bit 7, b
    bit(7, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x79() {
    // bit 7, c
    bit(7, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x7A() {
    // bit 7, d
    bit(7, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x7B() {
    // bit 7, e
    bit(7, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x7C() {
    // bit 7, h
    bit(7, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x7D() {
    // bit 7, l
    bit(7, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x7E() {
    // bit 7, (hl);
    bit(7, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x7F() {
    // bit 7, a
    bit(7, readMemory(indexedAddressForCurrentOpcode));
    cyclesTaken = 20;
}

void CPUZ80::indexBitOpcodeHandler0x80() {
    // res 0, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x81() {
    // res 0, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x82() {
    // res 0, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x83() {
    // res 0, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x84() {
    // res 0, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x85() {
    // res 0, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x86() {
    // res 0, (hl)
    writeMemory(indexedAddressForCurrentOpcode, res(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x87() {
    // res 0, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x88() {
    // res 1, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x89() {
    // res 1, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x8A() {
    // res 1, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x8B() {
    // res 1, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x8C() {
    // res 1, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x8D() {
    // res 1, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x8E() {
    // res 1, (hl)
    writeMemory(indexedAddressForCurrentOpcode, res(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x8F() {
    // res 1, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x90() {
    // res 2, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x91() {
    // res 2, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x92() {
    // res 2, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x93() {
    // res 2, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x94() {
    // res 2, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x95() {
    // res 2, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x96() {
    // res 2, (hl)
    writeMemory(indexedAddressForCurrentOpcode, res(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x97() {
    // res 2, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x98() {
    // res 3, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x99() {
    // res 3, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x9A() {
    // res 3, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x9B() {
    // res 3, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x9C() {
    // res 3, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x9D() {
    // res 3, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x9E() {
    // res 3, (hl)
    writeMemory(indexedAddressForCurrentOpcode, res(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0x9F() {
    // res 3, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA0() {
    // res 4, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA1() {
    // res 4, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA2() {
    // res 4, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA3() {
    // res 4, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA4() {
    // res 4, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA5() {
    // res 4, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA6() {
    // res 4, (hl)
    writeMemory(indexedAddressForCurrentOpcode, res(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA7() {
    // res 4, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA8() {
    // res 5, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xA9() {
    // res 5, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xAA() {
    // res 5, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xAB() {
    // res 5, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xAC() {
    // res 5, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xAD() {
    // res 5, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xAE() {
    // res 5, (hl)
    writeMemory(indexedAddressForCurrentOpcode, res(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xAF() {
    // res 5, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB0() {
    // res 6, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB1() {
    // res 6, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB2() {
    // res 6, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB3() {
    // res 6, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB4() {
    // res 6, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB5() {
    // res 6, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB6() {
    // res 6, (hl)
    writeMemory(indexedAddressForCurrentOpcode, res(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB7() {
    // res 6, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB8() {
    // res 7, b
    ldReg8(gpRegisters[cpuReg::BC].hi, res(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xB9() {
    // res 7, c
    ldReg8(gpRegisters[cpuReg::BC].lo, res(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xBA() {
    // res 7, d
    ldReg8(gpRegisters[cpuReg::DE].hi, res(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xBB() {
    // res 7, e
    ldReg8(gpRegisters[cpuReg::DE].lo, res(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xBC() {
    // res 7, h
    ldReg8(gpRegisters[cpuReg::HL].hi, res(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xBD() {
    // res 7, l
    ldReg8(gpRegisters[cpuReg::HL].lo, res(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xBE() {
    // res 7, (hl)
    writeMemory(indexedAddressForCurrentOpcode, res(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xBF() {
    // res 7, a
    ldReg8(gpRegisters[cpuReg::AF].hi, res(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC0() {
    // set 0, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC1() {
    // set 0, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC2() {
    // set 0, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC3() {
    // set 0, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC4() {
    // set 0, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC5() {
    // set 0, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC6() {
    // set 0, (hl)
    writeMemory(indexedAddressForCurrentOpcode, set(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC7() {
    // set 0, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(0, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC8() {
    // set 1, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xC9() {
    // set 1, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xCA() {
    // set 1, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xCB() {
    // set 1, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xCC() {
    // set 1, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xCD() {
    // set 1, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xCE() {
    // set 1, (hl)
    writeMemory(indexedAddressForCurrentOpcode, set(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xCF() {
    // set 1, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(1, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD0() {
    // set 2, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD1() {
    // set 2, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD2() {
    // set 2, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD3() {
    // set 2, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD4() {
    // set 2, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD5() {
    // set 2, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD6() {
    // set 2, (hl)
    writeMemory(indexedAddressForCurrentOpcode, set(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD7() {
    // set 2, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(2, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD8() {
    // set 3, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xD9() {
    // set 3, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xDA() {
    // set 3, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xDB() {
    // set 3, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xDC() {
    // set 3, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xDD() {
    // set 3, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xDE() {
    // set 3, (hl)
    writeMemory(indexedAddressForCurrentOpcode, set(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xDF() {
    // set 3, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(3, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE0() {
    // set 4, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE1() {
    // set 4, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE2() {
    // set 4, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE3() {
    // set 4, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE4() {
    // set 4, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE5() {
    // set 4, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE6() {
    // set 4, (hl)
    writeMemory(indexedAddressForCurrentOpcode, set(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE7() {
    // set 4, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(4, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE8() {
    // set 5, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xE9() {
    // set 5, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xEA() {
    // set 5, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xEB() {
    // set 5, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xEC() {
    // set 5, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xED() {
    // set 5, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xEE() {
    // set 5, (hl)
    writeMemory(indexedAddressForCurrentOpcode, set(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xEF() {
    // set 5, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(5, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF0() {
    // set 6, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF1() {
    // set 6, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF2() {
    // set 6, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF3() {
    // set 6, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF4() {
    // set 6, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF5() {
    // set 6, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF6() {
    // set 6, (hl)
    writeMemory(indexedAddressForCurrentOpcode, set(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF7() {
    // set 6, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(6, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF8() {
    // set 7, b
    ldReg8(gpRegisters[cpuReg::BC].hi, set(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xF9() {
    // set 7, c
    ldReg8(gpRegisters[cpuReg::BC].lo, set(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xFA() {
    // set 7, d
    ldReg8(gpRegisters[cpuReg::DE].hi, set(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xFB() {
    // set 7, e
    ldReg8(gpRegisters[cpuReg::DE].lo, set(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xFC() {
    // set 7, h
    ldReg8(gpRegisters[cpuReg::HL].hi, set(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xFD() {
    // set 7, l
    ldReg8(gpRegisters[cpuReg::HL].lo, set(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xFE() {
    // set 7, (hl)
    writeMemory(indexedAddressForCurrentOpcode, set(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}

void CPUZ80::indexBitOpcodeHandler0xFF() {
    // set 7, a
    ldReg8(gpRegisters[cpuReg::AF].hi, set(7, readMemory(indexedAddressForCurrentOpcode)));
    cyclesTaken = 23;
}