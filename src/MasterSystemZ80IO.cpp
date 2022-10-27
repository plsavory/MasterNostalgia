#include "MasterSystemZ80IO.h"

MasterSystemZ80IO::MasterSystemZ80IO(VDP *vdp, PSG *psg) {
    this->vdp = vdp;
    this->psg = psg;
}

void MasterSystemZ80IO::write(unsigned char address, unsigned char value) {
    // TODO handle the Game Gear I/O port map if I ever add support for it

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
        // TODO even address - write to VDP data port
        // TODO odd address - write to VDP control port
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
        // TODO even address - return SN76489 PSG V counter
        // TODO odd address - return SN76489 PSG H counter
        return 0xB0;
    }

    if (address <= 0xBF) {
        // TODO even address - return VDP data port contents
        // TODO odd address - return VDP status flags
        return 0x0;
    }

    // TODO even address - return I/O port A/B register
    // TODO odd address - return I/O port B/misc register
    return 0x0;
}