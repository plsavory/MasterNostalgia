#include "MasterSystemZ80IO.h"
#include <iostream>
#include "Utils.h"

//#define DEBUG_IO_WRITE
//#define DEBUG_IO_READ

MasterSystemZ80IO::MasterSystemZ80IO(VDP *vdp, PSG *psg) {
    this->vdp = vdp;
    this->psg = psg;
}

void MasterSystemZ80IO::write(unsigned char address, unsigned char value) {
    // TODO handle the Game Gear I/O port map if I ever add support for it

    #ifdef DEBUG_IO_WRITE
    std::cout<<"PORT "<<std::hex<<Utils::formatHexNumber(address)<<" WRITE: VAL: " << Utils::formatHexNumber(value)<<std::endl;
    #endif

    if (address <= 0x3F) {
        // TODO even address - write to memory control register
        // TODO odd address - write to I/O control register
        return;
    }

    if (address <= 0x7F) {
        // TODO - write to SN76489 PSG
        return;
    }

    if (address <= 0xBF) {

        if (address % 2) {
            // Write to VDP control port
            vdp->writeControlPort(value);
            return;
        }

        // Write to VDP data port
        vdp->writeDataPort(value);
        return;
    }

    // TODO even address - return I/O port A/B register
    // TODO odd address - return I/O port B/misc register
}

unsigned char MasterSystemZ80IO::read(unsigned char address) {
    if (address <= 0x3F) {
        // Return the last byte of the instruction which read the port
        // TODO or 0xFF if emulating Master System 2
        return address;
    }

    if (address <= 0x7F) {
        if (address % 2) {
            return vdp->readHCounter();
        }
        return vdp->readVCounter();
    }

    if (address <= 0xBF) {
        if (address % 2) {
            return vdp->readStatus();
        }

        return vdp->readDataPort();
    }

    if (address == 0xDC || address == 0xC0) {
        // Controller port 1 TODO implement
        /**
         *    bits - 0 for pressed, 1 for released
         *    7: Joypad 2 Down
         *    6: Joypad 2 Up
         *    5: Joypad 1 Fire B
         *    4: Joypad 1 Fire A
         *    3: Joypad 1 Right
         *    2: Joypad 1 Left
         *    1: Joypad 1 Down
         *    0: Joypad 1 Up
         */
        return 0xFF;
    }

    if (address == 0xDD || address == 0xC1) {
        // Controller port 2, also console region TODO implement
        /**
         *    7: Lightgun 2 + Nationalisation bit 2
         *    6: Lightgun 1 + Nationalisation bit 1
         *    5: unused
         *    4: Reset button
         *    3: Joypad 2 Fire B
         *    2: Joypad 2 Fire A
         *    1: Joypad 2 Right
         *    0: Joypad 2 Left
         */
         return 0xFF;
    }
    // TODO even address - return I/O port A/B register
    // TODO odd address - return I/O port B/misc register
    return 0x0;
}

bool MasterSystemZ80IO::isVDPRequestingInterrupt() {
    return vdp->isRequestingInterrupt();
}

void MasterSystemZ80IO::printVDPDebugInfo() {
    vdp->printDebugInfo();
}