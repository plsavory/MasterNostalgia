//
// Created by Peter Savory on 08/04/2023.
//

#include "MasterSystemController.h"

MasterSystemController::MasterSystemController() {
    for (int i = 0; i < 5; i++) {
        buttons[i] = 0x0;
    }
}

void MasterSystemController::setState(bool dPadUp, bool dPadDown, bool dPadLeft, bool dPadRight, bool buttonA,
                                      bool buttonB) {
    setButton(MasterSystemControllerButton::dPadUp, dPadUp);
    setButton(MasterSystemControllerButton::dPadDown, dPadDown);
    setButton(MasterSystemControllerButton::dPadLeft, dPadLeft);
    setButton(MasterSystemControllerButton::dPadRight, dPadRight);
    setButton(MasterSystemControllerButton::buttonA, buttonA);
    setButton(MasterSystemControllerButton::buttonB, buttonB);
}

void MasterSystemController::setButton(MasterSystemControllerButton button, bool value) {
    // TODO handle any input logging that we need for replays etc
    buttons[button] = value;
}

unsigned char MasterSystemController::getButtonValue(MasterSystemControllerButton button) {
    return buttons[button] ? 1 : 0;
}