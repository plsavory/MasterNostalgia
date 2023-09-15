#ifndef MEMORY_INCLUDED
#define MEMORY_INCLUDED

#include "Config.h"
#include "SMSSaveStateStructs.h"

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
    Memory(Cartridge *cart, Config *config);

    ~Memory();

    unsigned char read(unsigned short location);

    unsigned short read16Bit(unsigned short location);

    void write(unsigned short location, unsigned char value);

    void write(unsigned short location, unsigned short value);

    void writeMediaControlRegister(unsigned char value);

    void handleCRAMWriting();

    void init();

    void shutdown();

    MemorySaveStateData* getSaveStateData();

    void restoreState(MemorySaveStateData *data);

private:

    Cartridge *smsCartridge;
    Config *config;

    unsigned char ram[0x10000]{};

    void handleMemoryPaging(unsigned short location, unsigned char value);

    unsigned char ramBank[2][0x4000]{};
    int currentPage3RamBank;
    unsigned short memoryPages[3]{};

    unsigned char controlRegister;

    unsigned char readMedia(unsigned long location);

    void writeCRAM();

    void readCRAM();

    static std::vector<unsigned char> getCRAMDumpFileHeader();

    bool CRAMChangedSinceLastWrite;

    unsigned short framesSinceLastCRAMWrite;

    bool doesCRAMContainData();

    std::string getCRAMSaveFilePath();

};

#endif