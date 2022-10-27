#ifndef MASTALGIA_EMULATOR_H
#define MASTALGIA_EMULATOR_H

#include <iostream>
#include "Utils.h"
#include "MasterSystem.h"
#include "Exceptions.h"

/**
 * A class for handling loading/running different systems. Right now only the Master System is supported.
 */
class Emulator {
public:
    Emulator();

    ~Emulator();

    void init(const std::string &fileName);

    void run();

private:
    MasterSystem *system;
};

#endif