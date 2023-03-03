//
// Created by Peter Savory on 27/10/2022.
//

#ifndef MASTALGIA_MASTERSYSTEMZ80IO_H
#define MASTALGIA_MASTERSYSTEMZ80IO_H

#include "Z80IO.h"
#include "VDP.h"
#include "PSG.h"

class MasterSystemZ80IO : public Z80IO {

public:

    MasterSystemZ80IO(VDP *vdp, PSG *psg);

    void write(unsigned char address, unsigned char value) final;

    unsigned char read(unsigned char address) final;

    bool isVDPRequestingInterrupt() final;

    void printVDPDebugInfo() final;

private:

    VDP *vdp;

    PSG *psg;
};

#endif //MASTALGIA_MASTERSYSTEMZ80IO_H
