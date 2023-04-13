//
// Created by Peter Savory on 08/04/2023.
//

#ifndef MASTALGIA_MASTERSYSTEMCONTROLLER_H
#define MASTALGIA_MASTERSYSTEMCONTROLLER_H

enum MasterSystemControllerButton: int {
    dPadUp = 0,
    dPadDown = 1,
    dPadLeft = 2,
    dPadRight = 3,
    buttonA = 4,
    buttonB = 5
};

struct MasterSystemController {
    // TODO allow the state of this to be set from multiple input devices
public:

    MasterSystemController();

    void setState(bool dPadUp, bool dPadDown, bool dPadLeft, bool dPadRight, bool buttonA, bool buttonB);

    unsigned char getButtonValue(MasterSystemControllerButton button);
private:

    bool buttons[6];

    void setButton(MasterSystemControllerButton button, bool value);
};

#endif //MASTALGIA_MASTERSYSTEMCONTROLLER_H
