/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#ifndef SMS_VDP_H
#define SMS_VDP_H

#include "VDPDisplayMode.h"
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

// VDP Control Registers 3 and 4 are yf.

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

    sf::Uint8* getVideoOutput();

    bool isRequestingInterrupt();

    void printDebugInfo();

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

    unsigned short hCounter;

    unsigned char vCounter;

    unsigned char getCodeRegister() const;

    unsigned short getAddressRegister() const;

    void incrementAddress();

    void writeRegister();

    void renderScanline();

    unsigned char getMode();

    /**
     * Jumps the VCounter to another location if required
     * @return true if counter has jumped, otherwise false
     */
    bool handleVCounterJump(unsigned char currentVCounter);

    void setHCounter(unsigned short value);

    bool vRefresh;

    bool isVBlanking;

    unsigned char vScroll;

    VDPDisplayMode displayMode;

    void handleScanlineChange();

    unsigned char lineInterruptCounter;

    unsigned char vCounterJumpCount;

    unsigned short getSpriteAllocationTableBaseAddress();

    void renderSpritesMode2();

    void renderSpritesMode4();

    void renderBackgroundMode2();

    void renderBackgroundMode4();

    static unsigned char getColourValue(unsigned char rgb);

    //region Display output
    // TODO these could probably do with refactoring once multiple systems are supported. Might be useful to have a separate "display" class.
    sf::Uint8 *workingBuffer;

    sf::Uint8 *outputBuffer;

    void clearScreen();

    void fillVideoOutput();

    void putPixel(unsigned char x, unsigned char y, unsigned char r, unsigned char g, unsigned char b);

    bool doesPixelMatch(unsigned char x, unsigned char y, unsigned char r, unsigned char g, unsigned char b);

    static unsigned int getPixelBitmapIndex(unsigned char x, unsigned char y);
    //endregion

};
#endif

// 30th November = leaving date