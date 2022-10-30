/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#ifndef SMS_VDP_H
#define SMS_VDP_H

// Flag register enums, some won't be needed but will leave them here for reference
enum class StatusFlag : unsigned char {
    spriteCollision = 5,
    spriteOverflow = 6,
    vSyncInterruptPending = 7
};

enum VDPControlRegisters {
    misc1 = 0,
    misc2 = 1,
    baseNameTableAddress = 2,
    spriteAttributeTableBase = 5,
    spriteTileDataLocation = 6,
    overScanColour = 7,
    backgroundXScrollPosition = 8,
    backgroundYScrollPosition = 9,
    lineCounter = 10
};

enum class VDPControlRegister0 : unsigned char {
    useMode2 = 1,
    useMode4 = 2,
    offsetSpritesMinus8Pixels = 3,
    lineInterruptEnabled = 4,
    column0UsesOverScanColour = 5,
    column01DisableVerticalScrolling = 6,
    column24to31DisableVerticalScrolling = 7
};

enum class VDPControlRegister1 : unsigned char {
    zoomSprites = 0, // Render sprites at 2x their original size
    use16x16Sprites = 1, // 8x8 if this is not set
    unused2 = 2,
    use240ScanLines = 3, // TODO enforce that VDP mode 2 is set when used
    use224ScanLines = 4, // TODO enforce that VDP mode 2 is set when used
    vSyncInterruptsEnabled = 5,
    enableRendering = 6
};

enum class VDPControlRegister2 : unsigned char { // TODO probably won't need this - just write a function to build the base name table address
    unused = 0,
    nameTableBaseAddressBit11 = 1,
    nameTableBaseAddressBit12 = 2,
    nameTableBaseAddressBit13 = 3
};

// VDP Control Registers 3 and 4 are unused.

class VDP {
public:
    VDP();

    ~VDP();

    void execute(float cycles);

    /**
     * Writes value to one of the VDP registers
     * @param value
     */
    void writeControlPort(unsigned char value);

    /**
     * Writes value to VRAM
     * @param value
     */
    void writeDataPort(unsigned char value);

    unsigned char readStatus();

    unsigned char readDataPort();

    unsigned char readHCounter();

    unsigned char readVCounter();

private:

    /**
     * VDP Memory map for reference:
     * 0x0000 - 0x1FFF = Sprite / tile patterns (numbers 0 to 255)
     * 0x2000 - 0x37FF = Sprite / tile patterns (numbers 256 to 447)
     * 0x3800 - 0x3EFF = Name Table
     * 0x3F00 - 0x3FFF = Sprite Info Table
     */
    unsigned char vRAM[0x4000];

    unsigned char cRAM[0x20];

    unsigned char statusRegister;

    unsigned char registers[11];

    unsigned short getNameTableBaseAddress();

    unsigned short controlWord;

    unsigned char readBuffer;

    bool requestInterrupt;

    bool isSecondControlWrite;

    unsigned char hCounter;

    unsigned char vCounter;

    unsigned char getCodeRegister() const;

    unsigned short getAddressRegister() const;

    void incrementAddress();

    void writeRegister();

    unsigned char getMode();
};
#endif