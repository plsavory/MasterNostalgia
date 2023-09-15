//
// Created by Peter Savory on 08/04/2023.
//

#ifndef MasterNostalgia_MASTERSYSTEMINPUT_H
#define MasterNostalgia_MASTERSYSTEMINPUT_H

#include "MasterSystemController.h"
#include "InputInterface.h"

class MasterSystemInput {
public:

    MasterSystemInput(InputInterface *inputInterface);

    void setState();

    unsigned char readPortDC();

    unsigned char readPortDD();

private:
    InputInterface *inputInterface;
    MasterSystemController controllers[2];
    unsigned char resetButton;
    unsigned char pauseButton;
};

#endif //MasterNostalgia_MASTERSYSTEMINPUT_H
