/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#ifndef SMS_VDP_H
#define SMS_VDP_H

class VDP {
public:
    VDP();

    ~VDP();

    void execute(float cycles);
};
#endif