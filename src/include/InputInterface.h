//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MASTALGIA_INPUTINTERFACE_H
#define MASTALGIA_INPUTINTERFACE_H

#include "Config.h"

/**
 * This class provides a standard way to determine whether the user is attempting to perform an action, regardless of what
 * the control is bound to (Keyboard/Controller/Etc)
 */
class InputInterface {
public:

    InputInterface(Config *config);

    bool isDPadUpPressed(int port);

    bool isDPadDownPressed(int port);

    bool isDPadLeftPressed(int port);

    bool isDPadRightPressed(int port);

    bool isButtonAPressed(int port);

    bool isButtonBPressed(int port);

private:

    PlayerControlConfig *playerControls[2];

    bool isKeyboardKeyPressed(int port, PlayerControlKeyboardConfig::Actions action);

};

#endif //MASTALGIA_CONTROLINTERFACE_H
