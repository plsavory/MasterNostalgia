/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */
#include "VDP.h"
#include "Utils.h"

VDP::VDP() {
    for (auto &vRAMByte : vRAM) {
        vRAMByte = 0x0;
    }

    for (auto &cRamByte : cRAM) {
        cRamByte = 0x0;
    }

    for (auto &controlRegister: registers) {
        controlRegister = 0x0;
    }

    statusRegister = 0x0;
    controlWord = 0x0;
    isSecondControlWrite = false;
    readBuffer = 0x0;
    requestInterrupt = false;
    hCounter = 0x0;
    vCounter = 0x0;
}

VDP::~VDP() {

}

/**
 * [vdp::execute Update the graphics chip]
 * @param cycles [description]
 */
void VDP::execute(float cycles) {
    // TODO: Implement this
}

unsigned short VDP::getNameTableBaseAddress() {
    return (registers[VDPControlRegisters::baseNameTableAddress] & 0x0E) << 10;
}

void VDP::writeControlPort(unsigned char value) {

    if (isSecondControlWrite) {
        controlWord = (controlWord & 0x00FF) + (((unsigned short)value) << 8);
        isSecondControlWrite = false;

        switch (getCodeRegister()) {
            case 0:
                // Update the read buffer and increment the current memory address register
                readBuffer = vRAM[getAddressRegister()];
                incrementAddress();
                break;
            case 2:
                // Write to a register
                writeRegister();
                break;
            default:
                break;
        }

        return;
    }

    controlWord = (controlWord & 0xFF00) + value;

    isSecondControlWrite = true;

}

void VDP::writeRegister() {
    // Write the lower byte of the control word to the register represented by the top 4 bits of the control word
    unsigned char registerId = (controlWord >> 8) & 0xF;

    if (registerId > 11) {
        return; // Can't write to a register which doesn't exist.
    }

    registers[registerId] = (controlWord & 0xFF);

    if (registerId != 1) {
        return;
    }

    if (Utils::testBit(7, statusRegister) && Utils::testBit(5, registers[1])) {
        requestInterrupt = true;
    }
}

unsigned char VDP::getCodeRegister() const {
    return ((unsigned char)controlWord >> 14);
}

unsigned short VDP::getAddressRegister() const {
    return controlWord & 0x3FFF;
}

void VDP::incrementAddress() {
    if (controlWord == 0x3FFF) {
        controlWord &= 0xC000; // Wrap the memory address but keep the control word unchanged
        return;
    }

    ++controlWord;
}

void VDP::writeDataPort(unsigned char value) {
    isSecondControlWrite = false;
    unsigned short address = getAddressRegister();

    if (getCodeRegister() < 3) {
        vRAM[address] = value;
    } else {
        cRAM[address & 0x1F] = value; // cRAM is only 32 bytes in size - so discard bits that we don't need from address register value
    }

    readBuffer = value;
    incrementAddress();
}

unsigned char VDP::getMode() {
     return (Utils::testBit(2, registers[0x0]) << 3) +
            (Utils::testBit(3, registers[0x1]) << 2) +
            (Utils::testBit(1, registers[0x0]) << 1) +
            (Utils::testBit(4, registers[0x1]));

}

unsigned char VDP::readStatus() {
    unsigned char currentStatus = statusRegister;
    isSecondControlWrite = false;
    requestInterrupt = false;
    statusRegister &= (getMode() == 2 ? 0x2F : 0x1F);
    return currentStatus;
}

unsigned char VDP::readDataPort() {
    isSecondControlWrite = false;
    unsigned char currentReadBuffer = readBuffer;
    readBuffer = vRAM[getAddressRegister()];
    incrementAddress();
    return currentReadBuffer;
}

unsigned char VDP::readHCounter() {
    isSecondControlWrite = false;
    return hCounter;
}

unsigned char VDP::readVCounter() {
    isSecondControlWrite = false;
    return vCounter;
}