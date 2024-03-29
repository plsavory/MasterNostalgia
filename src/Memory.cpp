/*
Master System memory map:
0x0000-0x3FFF : Rom Slot 1
0x4000-0x7FFF : Rom Slot 2
0x8000-0xBFFF : Rom Slot 3 / RAM Slot
0xC000-0xDFFF : RAM
0xE000-0xFFFF : Mirrored RAM
 */
#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "Utils.h"

Memory::Memory(Cartridge *cart) {
    smsCartridge = cart;
    controlRegister = 0xBF; // Enable cartridge slot by default for now // TODO enable BIOS if one is detected

    // Clear memory
    for (unsigned char & i : ram) {
        i = 0x0;
    }

    // Clear RAM banks
    for (auto &bank: ramBank) {
        for (unsigned char &i: bank) {
            i = 0x0;
        }
    }

    for (unsigned short &memoryPage: memoryPages) {
        memoryPage = 0x0;
    }
    memoryPages[1] = 1;
    memoryPages[2] = 2;

    ramBanked = false;
    currentPage3RamBank = -1;
}

Memory::~Memory() = default;

/**
 * [read Read from system memory]
 * @param  location [Memory location to read from]
 * @return          [Value within memory at location]
 */
unsigned char Memory::read(unsigned short location) {
    // Handle mirroring
    if (location >= 0xFFFC) {
        location -= 0x2000;
    }

    if (!smsCartridge->isCodemasters() && location < 0x400) {
        return readMedia(location);
    }

    if (location < 0x4000) {
        // Page 1
        unsigned long address = location + (0x4000 * memoryPages[0]);
        return readMedia(address);
    }

    if (location < 0x8000) {
        // Page 2
        unsigned long address = location + (0x4000 * memoryPages[1]);
        address -= 0x4000; // Remove offset
        return readMedia(address);
    }

    if (location < 0xC000) {
        // Page 3 (or extra RAM)
        if (currentPage3RamBank >= 0) {
            return ramBank[currentPage3RamBank][location - 0x8000];
        }

        unsigned long address = location + (0x4000 * memoryPages[2]);
        address -= 0x8000;
        return readMedia(address);
    }

    return ram[location];
}

unsigned char Memory::readMedia(unsigned long location) {
    // TODO allow reading from BIOS, also determine which priority these should be if multiple flags are turned on
    if (!Utils::testBit(MemoryControlRegisterFlags::enableCartridgeSlot, controlRegister)) {
        return smsCartridge->read(location);
    }

    return 0xFF;
}

unsigned short Memory::read16Bit(unsigned short location) {
    return read(location) + (read(location+1) << 8);
}

void Memory::write(unsigned short location, unsigned short value) {
    write(location, (unsigned char)(value & 0x00FF));
    write(location+1, (unsigned char)(value >> 8));
}

/**
 * [write Writes to system memory]
 * @param location [Memory location to write to]
 * @param value    [The byte to write]
 */
void Memory::write(unsigned short location, unsigned char value) {

    handleMemoryPaging(location, value);

    if (location < 0x8000) {
        // Attempting to write to ROM, disallow this...
        return;
    }

    if (location < 0xC000) {
        // Allow a write if RAM is banked into this address range
        // TODO should the media/memory control register effect this also for when attempting to write to CART RAM?
        if (currentPage3RamBank < 0) {
            return;
        }

        ramBank[currentPage3RamBank][location - 0x8000] = value;
        return;
    }

    // If we've reached this point, it's all good to write to RAM
    ram[location] = value;

    // Handle mirrored addresses
    if (location >= 0xC000 && location < 0xDFFC) {
        ram[location - 0x2000] = value;
        return;
    }

    if (location >= 0xE000) {
        ram[location - 0x2000] = value;
        return;
    }

}

/**
 * [Memory::memoryPage Perform a memory page operation]
 * @param Codemasters [Whether the game is codemasters or not]
 */
void Memory::handleMemoryPaging(unsigned short location, unsigned char value) {

    if (!smsCartridge->isCodemasters()) {

        if (location < 0xFFFC) {
            return;
        }

        unsigned short page = value & smsCartridge->getBankMask();

        switch (location) {
            case 0xFFFC:
                if (Utils::testBit(3, value)) {
                    if (Utils::testBit(2, value)) {
                        currentPage3RamBank = 1;
                    } else {
                        currentPage3RamBank = 0;
                    }
                } else {
                    currentPage3RamBank = -1;
                }
                break;
            case 0xFFFD:
                memoryPages[0] = page;
                break;
            case 0xFFFE:
                memoryPages[1] = page;
                break;
            case 0xFFFF:
                // ROM banking in slot 3 - Only allow if there is no RAM there
                if (!Utils::testBit(3, ram[0xFFFC])) {
                    memoryPages[2] = page;
                }
                break;
            default:
                break;
        }
        return;
    }

    if (location == 0x0 || location == 0x4000 || location == 0x8000) {
        unsigned short page = value & smsCartridge->getBankMask();

        switch (location) {
            case 0x0:
                memoryPages[0] = page;
                break;
            case 0x4000:
                memoryPages[1] = page;
                break;
            case 0x8000:
                memoryPages[2] = page;
                break;
            default:
                break;
        }
    }
}

void Memory::writeMediaControlRegister(unsigned char value) {
    controlRegister = value;
}
