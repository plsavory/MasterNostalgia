//
// Created by Peter Savory on 27/10/2022.
//

#ifndef MASTALGIA_MASTERSYSTEMZ80IO_H
#define MASTALGIA_MASTERSYSTEMZ80IO_H

#include "Z80IO.h"
#include "VDP.h"
#include "PSG.h"
#include "Cartridge.h"
#include "Memory.h"
#include "MasterSystemInput.h"

class MasterSystemZ80IO final : public Z80IO {

public:

    MasterSystemZ80IO(VDP *vdp, PSG *psg, Memory *memory, MasterSystemInput *input);

    ~MasterSystemZ80IO() = default;

    void write(unsigned char address, unsigned char value) final;

    unsigned char read(unsigned char address) final;

    bool isVDPRequestingInterrupt() final;

    void printVDPDebugInfo() final;

private:

    VDP *vdp;

    PSG *psg;

    Memory *memory;

    MasterSystemInput *input;
};

#endif //MASTALGIA_MASTERSYSTEMZ80IO_H
