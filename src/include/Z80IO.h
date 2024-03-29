//
// Created by Peter Savory on 27/10/2022.
//

#ifndef MasterNostalgia_Z80IO_H
#define MasterNostalgia_Z80IO_H

class Z80IO {
public:

    virtual void write(unsigned char address, unsigned char value) = 0;

    virtual unsigned char read(unsigned char address) = 0;

    virtual bool isVDPRequestingInterrupt() = 0;

    virtual void printVDPDebugInfo() = 0;
};

#endif //MasterNostalgia_Z80IO_H
