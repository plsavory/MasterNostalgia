//
// Created by Peter Savory on 23/06/2023.
//

#include "InputInterface.h"

InputInterface::InputInterface(Config *config) {
    playerControls[0] = config->getPlayer1ControlConfig();
    playerControls[1] = config->getPlayer2ControlConfig();
}

bool InputInterface::isKeyboardKeyPressed(int port, PlayerControlKeyboardConfig::Actions action) {

    if (!playerControls[port]) {
        return false;
    }

    PlayerControlKeyboardConfig *keyboardConfig = playerControls[port]->getKeyboardConfig();

    return keyboardConfig && sf::Keyboard::isKeyPressed(keyboardConfig->getBind(action));
}

bool InputInterface::isDPadUpPressed(int port) {
    // TODO check for gamepad controls
    return isKeyboardKeyPressed(port, PlayerControlKeyboardConfig::Actions::dPadUp);
}

bool InputInterface::isDPadDownPressed(int port) {
    // TODO check for gamepad controls
    return isKeyboardKeyPressed(port, PlayerControlKeyboardConfig::Actions::dPadDown);
}

bool InputInterface::isDPadLeftPressed(int port) {
    // TODO check for gamepad controls
    return isKeyboardKeyPressed(port, PlayerControlKeyboardConfig::Actions::dPadLeft);
}

bool InputInterface::isDPadRightPressed(int port) {
    // TODO check for gamepad controls
    return isKeyboardKeyPressed(port, PlayerControlKeyboardConfig::Actions::dPadRight);
}

bool InputInterface::isButtonAPressed(int port) {
    // TODO check for gamepad controls
    return isKeyboardKeyPressed(port, PlayerControlKeyboardConfig::Actions::a);
}

bool InputInterface::isButtonBPressed(int port) {
    // TODO check for gamepad controls
    return isKeyboardKeyPressed(port, PlayerControlKeyboardConfig::Actions::b);
}