/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */
#include "VDP.h"
#include "Utils.h"

VDP::VDP(Z80InterruptBus *interruptBus) {
    for (auto &vRAMByte : vRAM) {
        vRAMByte = 0x0;
    }

    for (auto &cRamByte : cRAM) {
        cRamByte = 0x0;
    }

    for (auto &controlRegister: registers) {
        controlRegister = 0x0;
    }

    this->interruptBus = interruptBus;
    statusRegister = 0x0;
    controlWord = 0x0;
    isSecondControlWrite = false;
    readBuffer = 0x0;
    requestInterrupt = false;
    hCounter = 0x0;
    vCounter = 0x0;
    vRefresh = false;
    isVBlanking = false;
    vCounterJumpCount = 0;
    displayMode = VDPDisplayMode::getDisplayMode(SMSDisplayMode::NTSCSmall); // TODO should this be the default? just using it for now.
    pixels = new sf::Uint8[256 * 224 * 4];

    for (int i = 0; i <= ((256 * 224) * 4); i += 4) {
        pixels[i] = 128; // R
        pixels[i + 1] = 128; // G
        pixels[i + 2] = 128; // B
        pixels[i + 3] = 255; // A
    }
}

VDP::~VDP() {

}


void VDP::execute(float cycles) {
    unsigned short currentHCount = hCounter;
    int vdpCycles = (int)cycles * 2;
    hCounter = (int)(hCounter + cycles) % 685;

    if (currentHCount + vdpCycles <= 684) {
        return;
    }

    handleScanlineChange();

    if (Utils::testBit(7, statusRegister) && Utils::testBit(5, registers[0x1])) {
        requestInterrupt = true;
    }

    if (requestInterrupt) {
        requestInterrupt = false;
        interruptBus->raiseInterrupt(InterruptType::INT);
    }
}

void VDP::handleScanlineChange() {

    // End of the current scanline
    hCounter = 0;
    unsigned char currentVCounter = vCounter;
    ++vCounter;

    // Handle VCounter timing/increment events
    if (currentVCounter == 255) {
        // End of vertical refresh - start rendering the next frame
        vCounter = 0;
        vCounterJumpCount = 0; // Ensure that we don't end up moving the VCounter back every time, should be done once per frame
        vRefresh = true;
        // TODO draw this scanline to the screen
    } else {
        handleVCounterJump(currentVCounter);
    }

    if (vCounterJumpCount == 0 && vCounter == displayMode.getActiveDisplayEnd()) {
        // Entering vertical refresh on the next scanline if we've reached the end of the active display and the vcounter has not jumped
        isVBlanking = true;
        Utils::setBit(7, true, statusRegister);
    }

    if (vCounter <= displayMode.getActiveDisplayEnd()) {

        if (vCounter != displayMode.getActiveDisplayEnd()) {
            // Active display - render this scanline
            renderScanline();
        }

        // TODO handle line interrupt timing

        // Determine whether the line interrupt counter is going to underflow and decrement it
        if ((lineInterruptCounter--) == 0) {
            lineInterruptCounter = registers[0xA]; // Reload the counter - as the program may later check for another scanline on this frame

            if (Utils::testBit(4, registers[0x0])) {
                requestInterrupt = true;
            }
        }
    }

    if (vCounter >= displayMode.getActiveDisplayEnd()) {
        // Inactive display area
        if (vCounter != displayMode.getActiveDisplayEnd()) {
            // Line interrupt counter should be loaded on the first scanline after the active display period
            lineInterruptCounter = registers[0xA];
        }

        vScroll = registers[0x9];

        // Allow the screen resolution to change
        // TODO handle PAL modes
        switch (getMode()) {
            case 11:
                displayMode = VDPDisplayMode::getDisplayMode(SMSDisplayMode::NTSCMedium);
                break;
            case 14:
                displayMode = VDPDisplayMode::getDisplayMode(SMSDisplayMode::NTSCLarge);
                break;
            default:
                displayMode = VDPDisplayMode::getDisplayMode(SMSDisplayMode::NTSCSmall);
                break;
        }
    }

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
    return (hCounter >> 1) & 0xFF;
}

unsigned char VDP::readVCounter() {
    isSecondControlWrite = false;
    return vCounter;
}

bool VDP::handleVCounterJump(unsigned char currentVCounter) {

    std::vector<VDPDisplayModeVCounterJump> vCounterJumps = displayMode.getVCounterJumps();

    if (vCounterJumpCount >= vCounterJumps.size()) {
        // We've already jumped enough times - do nothing
        return false;
    }

    VDPDisplayModeVCounterJump jump = vCounterJumps[vCounterJumpCount];

    if (currentVCounter != jump.getFrom()) {
        return false; // Not the correct scanline - don't change vcounter
    }

    vCounter = jump.getTo();
    ++vCounterJumpCount;

    return true;
}

void VDP::renderScanline() {

}

sf::Uint8* VDP::getVideoOutput() {
    return pixels;
}