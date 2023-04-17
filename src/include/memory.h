/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */
#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED

enum MemoryControlRegisterFlags : int{
    unknown0 = 0,
    unknown1 = 1,
    enableIO = 2,
    enableBIOS = 3,
    enableRAM = 4,
    enableCardSlot = 5,
    enableCartridgeSlot = 6,
    enableExpansionSlot = 7
};

class Memory {
public:
    Memory(Cartridge *cart);

    ~Memory();

    unsigned char read(unsigned short location);

    unsigned short read16Bit(unsigned short location);

    void write(unsigned short location, unsigned char value);

    void write(unsigned short location, unsigned short value);

    void writeMediaControlRegister(unsigned char value);

private:
    Cartridge *smsCartridge;
    unsigned char ram[0x10000]{};
    bool ramBanked; // Does cartridge include additional RAM?
    void memoryPage(bool Codemasters, unsigned short location, unsigned char value);

    unsigned char ramBank[2][0x4000]{};
    int currentPage3RamBank;
    unsigned short memoryPages[3]{};

    unsigned char controlRegister;

    unsigned char readMedia(unsigned long location);
};

#endif