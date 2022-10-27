/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#ifndef SMS_PSG_H
#define SMS_PSG_H

class PSG {
public:
    PSG();

    ~PSG();

    void execute(int cycles);

private:
};

#endif