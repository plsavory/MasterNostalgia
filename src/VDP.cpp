#include "VDP.h"
#include "Utils.h"
#include "Exceptions.h"
#include "iostream"

VDP::VDP() {
    for (auto &vRAMByte : vRAM) {
        vRAMByte = 0x0;
    }

    for (auto &cRamByte : cRAM) {
        cRamByte = 0x0;
    }

    // Set registers to initial values so that we can run without a BIOS. Source: https://github.com/maxim-zhao/smsbioses/blob/master/1.0/Bios10.asm
    for (auto &controlRegister: registers) {
        controlRegister = 0x0;
    }

    mode2Colours = {
            {0, 0, 0},
            {0, 0, 0},
            {33, 200, 66},
            {94, 220, 120},
            {84, 85, 237},
            {125, 118, 252},
            {212, 82, 77},
            {66, 235, 245},
            {252, 85, 84},
            {255, 121, 120},
            {212, 193, 84},
            {230, 206, 128},
            {33, 176, 59},
            {201, 91, 186},
            {204, 204, 204},
            {255, 255, 255}
    };

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
    workingBuffer = new sf::Uint8[256 * 224 * 4];
    outputBuffer = new sf::Uint8[256 * 224 * 4];
    vScroll = 0;
    lineInterruptCounter = 0;
    clearScreen();
    fillVideoOutput();
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
        renderScanline();
        fillVideoOutput(); // Fill the video output workingBuffer with the current full frame
        clearScreen();
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
    if (displayMode.getActiveDisplayEnd() == 192) {
        // Ignore bit 0 and the top nibble
        return ((unsigned short)(registers[0x2] & 0xE)) << 10;
    }

    return (((unsigned short)(registers[0x2] & 0x0C)) << 10) | 0x700;
}

void VDP::writeControlPort(unsigned char value) {

    if (isSecondControlWrite) {
        controlWord = (controlWord & 0x00FF) + (((unsigned short)value) << 8);

        isSecondControlWrite = false;

        unsigned char controlRegister = getCodeRegister();

        switch (controlRegister) {
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
    return (unsigned char)(controlWord >> 14);
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

bool VDP::isRequestingInterrupt() {
    return requestInterrupt;
}

void VDP::renderScanline() {
    if (getMode() == 2) {
        renderSpritesMode2();
        renderBackgroundMode2();
    } else {
        renderSpritesMode4();
        renderBackgroundMode4();
    }
}

sf::Uint8* VDP::getVideoOutput() {
    return outputBuffer;
}

unsigned short VDP::getSpriteAllocationTableBaseAddress() {
    return (registers[0x5] & 0x7E) << 7;
}

void VDP::renderSpritesMode2() {
    unsigned char spriteSize = Utils::testBit(1, registers[0x1]) ? 16 : 8;
    bool zoomSprites = Utils::testBit(0, registers[0x1]);

    unsigned char spriteSizeToDraw = spriteSize;

    if (zoomSprites) {
        spriteSizeToDraw *= 2;
    }

    unsigned short baseAddress = getSpriteAllocationTableBaseAddress();

    int spriteCount = 0;

    for (int i = 0; i < 32; i++) {

        unsigned short spriteAddress = baseAddress + (i*4);

        int y = vRAM[spriteAddress];

        if (y == 0xD0) {
            break;
        }

        if (y > 0xD0) {
            y -= 0x100;
        }

        y += 1;

        if (vCounter < y) {
            continue;
        }

        if (vCounter >= y + spriteSize) {
            continue;
        }

        // Sprite is on this scanline
        if (spriteCount == 4) {
            // Set sprite overflow flag
            Utils::setBit(6, true, statusRegister);
            break;
        }

        spriteCount++;

        int x = vRAM[spriteAddress + 1];

        unsigned short patternId = vRAM[spriteAddress + 2];
        unsigned char colour = vRAM[spriteAddress + 3] & 0xF;

        if (vRAM[spriteAddress + 3] & 0x80) {
            x -= 32;
        }

        unsigned short patternAddress = (registers[0x4] & 0x7) << 11;

        if (spriteSize == 8) {
            patternAddress += (patternId * 8);
        } else {
            patternAddress += (patternId & 252) * 8;
        }

        patternAddress += (vCounter - y);

        unsigned short pattern;

        if (spriteSize == 8) {
            pattern = vRAM[patternAddress];
        } else {
            pattern = (vRAM[patternAddress] << 8) + (vRAM[patternAddress + 16]);
        }

        // Render each pixel of the sprite
        int currentDataPixel = 0;

        for (unsigned char xPixel = 0; xPixel < spriteSizeToDraw; xPixel++) {

            if (x + xPixel > 255) {
                // Don't try to draw off screen
                break;
            }

            unsigned int pixelIndex = getPixelBitmapIndex(x + xPixel, vCounter);

            // TODO test if below logic is correct for zoomed sprites, when I find something that uses them.
            unsigned char dataPixelIncrement = ((!zoomSprites || xPixel % 2 == 1)) ? 1 : 0;

            if (!(pattern & (1 << ((spriteSizeToDraw - 1) - currentDataPixel)))) {
                currentDataPixel += dataPixelIncrement;
                continue;
            }

            if (isPixelUsed(pixelIndex)) {
                // Flag a sprite collision
                Utils::setBit(5, true, statusRegister);
                currentDataPixel += dataPixelIncrement;
                continue;
            }

            putPixel(pixelIndex, mode2Colours[colour].r, mode2Colours[colour].g, mode2Colours[colour].b);
            currentDataPixel += dataPixelIncrement;

        }

    }

}

void VDP::renderBackgroundMode2() {

}

void VDP::renderSpritesMode4() {

    bool zoomSprites = Utils::testBit(0, registers[0x1]);
    bool spriteSize8x16 = Utils::testBit(1, registers[0x1]);
    bool shiftLeft = Utils::testBit(3, registers[0x0]);

    unsigned char height = spriteSize8x16 ? 16 : 8;

    if (zoomSprites) {
        height *= 2;
    }

    unsigned short baseAddress = getSpriteAllocationTableBaseAddress();

    int spriteCount = 0;

    for (int i = 0; i < 64; i++) {

        // Sprite format: byte0 = y, byte1 = x, byte2 = unused, byte3 = pattern id
        int y = vRAM[baseAddress + i];

        if (y == 0xD0 && (displayMode.getActiveDisplayEnd() == 192)) {
            break;
        }

        if (y > 0xD0) {
            y -= 0x100;
        }

        y += 1;

        if (vCounter < y) {
            continue;
        }

        if (vCounter >= y + height) {
            continue;
        }

        // Sprite falls within this scanline
        if (spriteCount == 8) {
            // Set sprite overflow flag
            Utils::setBit(6, true, statusRegister);
            break;
        }

        spriteCount += 1;

        if (!Utils::testBit(6, registers[0x1])) {
            continue;
        }

        unsigned char x = vRAM[baseAddress + 128 + (i*2)];

        if (shiftLeft) {
            x -= 8;
        }

        // Fetch and draw the bitmap data for this sprite
        unsigned short patternId = vRAM[baseAddress + 128 + (i*2) + 1];

        if (Utils::testBit(2, registers[0x6])) {
            // Using second pattern table
            patternId += 256;
        }

        if (spriteSize8x16 && y-vCounter < 9) {
            /* In 8x16 mode, force the top half of the sprite's tile id to be the nearest-lowest even number,
             * as the tile immediately after this is now considered to be part of the same sprite and can't be drawn
             * independently as the first 8 pixels of another.*/
            patternId &= 0xFFFE;
        }

        unsigned short patternAddress = (patternId * 32) + (4 * (vCounter - y));

        unsigned char pattern1 = vRAM[patternAddress];
        unsigned char pattern2 = vRAM[patternAddress+1];
        unsigned char pattern3 = vRAM[patternAddress+2];
        unsigned char pattern4 = vRAM[patternAddress+3];

        for (unsigned char xPixel = 0; xPixel < 8; xPixel++) {

            unsigned int pixelIndex = getPixelBitmapIndex(x + xPixel, vCounter);

            if (x + xPixel > 255) {
                // Don't try to draw off screen
                break;
            }

            unsigned char pixelDataIndex = 7 - xPixel;

            unsigned char paletteId = (Utils::testBit(pixelDataIndex, pattern4) << 3) |
                                      (Utils::testBit(pixelDataIndex, pattern3) << 2) |
                                      (Utils::testBit(pixelDataIndex, pattern2) << 1) |
                                      (Utils::testBit(pixelDataIndex, pattern1));

            if (paletteId == 0) {
                continue;
            }

            if (isPixelUsed(pixelIndex)) {
                // Flag a sprite collision
                Utils::setBit(5, true, statusRegister);
                continue;
            }

            unsigned char rgb = cRAM[paletteId + 16];
            unsigned char r = getColourValue(rgb & 0x3);
            unsigned char g = getColourValue((rgb >> 2) & 0x3);
            unsigned char b = getColourValue((rgb >> 4) & 0x3);

            putPixel(pixelIndex, r, g, b);
        }
    }
}

void VDP::renderBackgroundMode4() {

    if (!Utils::testBit(6, registers[0x1])) {
        return;
    }

    unsigned short nameTableBaseAddress = getNameTableBaseAddress();

    unsigned char vScrollTileOffset = vScroll >> 3;
    unsigned char hTileOffset = registers[0x8] >> 3;
    unsigned char vScrollPixelOffset = vScroll & 0x7;
    unsigned char hScrollPixelOffset = registers[0x8] & 0x7;

    unsigned char row = vCounter / 8;

    bool limitVScroll = Utils::testBit(7, registers[0]);
    bool limitHScroll = Utils::testBit(6, registers[0]);

    // If this is set, the first column needs to be drawn as the background colour
    bool maskFirstColumn = Utils::testBit(5, registers[0]);

    for (int column = 0; column < 32; column++) {
        // Draw each tile to the screen
        for (int pixelCounter = 0; pixelCounter < 8; pixelCounter++) {

            // We should not allow horizontal scrolling here if we are on the first row and the appropriate flag is not set
            bool allowHScroll = (row > 1 || !limitHScroll);

            int onScreenPixelX = column * 8 + pixelCounter; // Where we will draw the actual pixel on screen

            // When scrolling horizontally, this can differ from the actual pixel screen location, as we need the workingBuffer to be offset from their position
            int xPixelDataOffset = onScreenPixelX;

            if (allowHScroll) {
                // Should be the first pixel of the starting column * tile width + the current screen pixel + the fine scroll offset
                onScreenPixelX = ((hTileOffset * 8) + pixelCounter + hScrollPixelOffset) % 256;
            }

            bool allowVScroll = !(limitVScroll && (xPixelDataOffset/8) > 23);

            int currentVRow = row;

            if (allowVScroll) {
                currentVRow += vScrollTileOffset;

                // We might overshoot the end of this tile by adding fine scroll, so check for that
                if (((vCounter % 8) + vScrollPixelOffset) > 7) {
                    ++currentVRow;
                }

                // Wrap the pixel data if we have exceeded the maximum number of rows
                currentVRow = currentVRow % (displayMode.getActiveDisplayEnd() == 192 ? 28 : 32);
            }

            unsigned short nameTableOffsetAddress = nameTableBaseAddress + (currentVRow * 64) + (column * 2);

            // AND the contents of bit 0 of register 2 with bit 10 of the name table offset address, Ys apparently relies on this.
            nameTableOffsetAddress |= (nameTableOffsetAddress & 0x400) & ((registers[0x2] & 0x1) << 10);

            unsigned short tileData = vRAM[nameTableOffsetAddress] + (vRAM[nameTableOffsetAddress+1] << 8);

            bool isHighPriority = Utils::testBit(12, tileData);
            bool useSpritePalette = Utils::testBit(11, tileData);
            bool verticalFlip = Utils::testBit(10, tileData);
            bool horizontalFlip = Utils::testBit(9, tileData);
            unsigned short tileId = tileData & 0x1FF;

            unsigned char pixelRowOffset = vCounter;

            if (allowVScroll) {
                pixelRowOffset += vScrollPixelOffset;
            }

            pixelRowOffset = pixelRowOffset % 8;

            if (verticalFlip) {
                pixelRowOffset = 7 - pixelRowOffset;
            }

            // Each pattern is 32 bytes in size, and each line is 4 bytes. Same as sprites.
            unsigned short patternMemoryAddress = (tileId * 32) + (pixelRowOffset * 4);

            unsigned char pattern1 = vRAM[patternMemoryAddress];
            unsigned char pattern2 = vRAM[patternMemoryAddress+1];
            unsigned char pattern3 = vRAM[patternMemoryAddress+2];
            unsigned char pattern4 = vRAM[patternMemoryAddress+3];

            // Get the colour to draw for this pixel
            int colourIndex = horizontalFlip ? pixelCounter : (7 - pixelCounter);

            unsigned char paletteId = (Utils::testBit(colourIndex, pattern4) << 3) |
                                      (Utils::testBit(colourIndex, pattern3) << 2) |
                                      (Utils::testBit(colourIndex, pattern2) << 1) |
                                      (Utils::testBit(colourIndex, pattern1));

            bool isMasking = false;

            if (onScreenPixelX < 8 && maskFirstColumn) {
                // Draw the background colour instead of the colour of the tile
                isMasking = true;
                paletteId = registers[0x7] & 15;
                useSpritePalette = true;
            }

            if (paletteId == 0) {
                // Can't be high priority if this pixel is transparent
                isHighPriority = false;
            }

            if (useSpritePalette) {
                paletteId+=16;
            }

            unsigned char rgb = cRAM[paletteId];
            unsigned char r = getColourValue(rgb & 0x3);
            unsigned char g = getColourValue((rgb >> 2) & 0x3);
            unsigned char b = getColourValue((rgb >> 4) & 0x3);

            if (xPixelDataOffset > 255) {
                break;
            }

            unsigned int pixelIndex = getPixelBitmapIndex(onScreenPixelX, vCounter);

            if (!isMasking && !isHighPriority && isPixelUsed(pixelIndex)) {
                // This isn't a high priority pixel and there is something here, so don't draw it.
                continue;
            }

            putPixel(pixelIndex, r, g, b);
        }

        hTileOffset = (hTileOffset + 1) % 32;
    }
}

unsigned char VDP::getColourValue(unsigned char rgb) {
    // Convert the Master System's colour values into something that we can draw to the screen
    switch (rgb) {
        case 0:
            return 0;
        case 1:
            return 85;
        case 2:
            return 170;
        case 3:
            return 255;
        default:
            return 0;
    }
}

//region Display output
void VDP::clearScreen() {
    for (int i = 0; i <= ((256 * 224) * 4); i += 4) {
        workingBuffer[i] = 0; // R
        workingBuffer[i + 1] = 0; // G
        workingBuffer[i + 2] = 0; // B
        workingBuffer[i + 3] = 0; // A
    }
}

void VDP::fillVideoOutput() {
    for (int i = 0; i <= ((256 * 224) * 4); i += 4) {
        outputBuffer[i] = workingBuffer[i]; // R
        outputBuffer[i + 1] = workingBuffer[i + 1]; // G
        outputBuffer[i + 2] = workingBuffer[i + 2]; // B
        outputBuffer[i + 3] = workingBuffer[i + 3]; // A
    }
}

void VDP::putPixel(unsigned long index, unsigned char r, unsigned char g, unsigned char b) {
    workingBuffer[index] = r;
    workingBuffer[index + 1] = g;
    workingBuffer[index + 2] = b;
    workingBuffer[index + 3] = 255;
}

bool VDP::isPixelUsed(unsigned long index) {
    return workingBuffer[index + 3] == 255;
}

inline unsigned int VDP::getPixelBitmapIndex(unsigned char x, unsigned char y) {
    return ((y * 256) + x) * 4;
}
//endregion

void VDP::printDebugInfo() {
    // Video Mode
    std::cout << "Video Mode: " << Utils::formatHexNumber(displayMode.getActiveDisplayEnd()) << std::endl << std::endl;

    // Registers
    std::cout << "Registers: " << std::endl << std::endl;

    for (unsigned char i = 0; i < 11; i++) {
        std::cout << "Register " << Utils::formatHexNumber(i) << ": " << Utils::formatHexNumber(registers[i]) << std::endl;
    }

    std::cout << "Register Status: " << Utils::formatHexNumber(statusRegister) << std::endl;
    std::cout << "Register HCounter: " << Utils::formatHexNumber(hCounter) << std::endl;
    std::cout << "Register VCounter: " << Utils::formatHexNumber(vCounter) << std::endl;

    std::cout << std::endl << std::endl;

    // Misc
    std::cout << "Misc: " << std::endl << std::endl;
    std::cout << "Tileset nametable address: " << Utils::formatHexNumber(getNameTableBaseAddress()) << std::endl;
    std::cout << "Sprite Allocation Table Address: " << Utils::formatHexNumber(getSpriteAllocationTableBaseAddress()) << std::endl;

    std::cout << std::endl << std::endl;

    // VRAM
    std::cout<<"VRAM: " << std::endl << std::endl;

    for (unsigned short i = 0x0; i < 0x4000; i += 0x10) {
        std::cout<<Utils::formatHexNumber(i) << "  ";
        for (unsigned char j = 0; j <= 0xF; j++) {
            std::cout<<Utils::formatHexNumber(vRAM[i + j]);

            if (j < 0xF) {
                std::cout<<" ";
            }
        }
        std::cout<<std::endl;
    }

    std::cout << std::endl << std::endl;

    // CRAM
    std::cout<<"CRAM: " << std::endl << std::endl;
    for (unsigned short i = 0x0; i < 0x20; i += 0x10) {
        std::cout<<Utils::formatHexNumber(i) << "  ";
        for (unsigned char j = 0; j <= 0xF; j++) {
            std::cout<<Utils::formatHexNumber(cRAM[i + j]);

            if (j < 0xF) {
                std::cout<<" ";
            }
        }
        std::cout<<std::endl;
    }
}

VDPDisplayMode VDP::getDisplayMode() {
    return displayMode;
}