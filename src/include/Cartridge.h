#ifndef CARTRIDGE_INCLUDED
#define CARTRIDGE_INCLUDED

#define MAX_CARTRIDGE_SIZE 0x400000

enum CartridgeRegion {
    Japan, USA, Europe, Unknown
};

class Cartridge {
public:
    Cartridge();

    ~Cartridge();

    bool load(std::string fileName);

    bool isCodemasters();

    unsigned char read(unsigned long location);

    unsigned char getBankMask();

    std::string getROMFilePath();

    std::string getROMFileName();

private:

    bool isCodemastersCart;

    unsigned char cartridgeData[MAX_CARTRIDGE_SIZE];

    void clearCartridge();

    CartridgeRegion region;

    void determineCartridgeSize(size_t ROMSize);

    unsigned char bankMask;

    std::string romFilePath;
};

#endif