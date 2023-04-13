//
// Created by Peter Savory on 08/04/2023.
//

#ifndef MASTALGIA_MASTERSYSTEMINPUT_H
#define MASTALGIA_MASTERSYSTEMINPUT_H

#include "MasterSystemController.h"

class MasterSystemInput {
public:

    MasterSystemInput();

    void setState();

    unsigned char readPortDC();

    unsigned char readPortDD();

private:
    MasterSystemController controllers[2];
    unsigned char resetButton;
    unsigned char pauseButton;
};

#endif //MASTALGIA_MASTERSYSTEMINPUT_H
