//
// Created by Peter Savory on 08/04/2023.
//

#include "MasterSystemInput.h"
#include "SFML/Window.hpp"

MasterSystemInput::MasterSystemInput() {

}

void MasterSystemInput::setState() {
    // TODO allow customisable controls
    resetButton = 0x0;
    pauseButton = sf::Keyboard::isKeyPressed(sf::Keyboard::Return);

    controllers[0].setState(
            sf::Keyboard::isKeyPressed(sf::Keyboard::Up),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Down),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Left),
            sf::Keyboard::isKeyPressed(sf::Keyboard::Right),
            sf::Keyboard::isKeyPressed(sf::Keyboard::A),
            sf::Keyboard::isKeyPressed(sf::Keyboard::S)
            );

    // TODO support the second controller
    controllers[1].setState(
            0,
            0,
            0,
            0,
            0,
            0
            );
}

unsigned char MasterSystemInput::readPortDC() {
    return ~ (
            controllers[0].getButtonValue(MasterSystemControllerButton::dPadUp) +
            (controllers[0].getButtonValue(MasterSystemControllerButton::dPadDown) << 1) +
            (controllers[0].getButtonValue(MasterSystemControllerButton::dPadLeft) << 2) +
            (controllers[0].getButtonValue(MasterSystemControllerButton::dPadRight) << 3) +
            (controllers[0].getButtonValue(MasterSystemControllerButton::buttonA) << 4) +
            (controllers[0].getButtonValue(MasterSystemControllerButton::buttonB) << 5) +
            (controllers[1].getButtonValue(MasterSystemControllerButton::dPadUp) << 6) +
            (controllers[1].getButtonValue(MasterSystemControllerButton::dPadDown) << 7)
        );
}

unsigned char MasterSystemInput::readPortDD() {
    return ~ (
            controllers[1].getButtonValue(MasterSystemControllerButton::dPadLeft) +
            (controllers[1].getButtonValue(MasterSystemControllerButton::dPadRight) << 1) +
            (controllers[1].getButtonValue(MasterSystemControllerButton::buttonA) << 2) +
            (controllers[1].getButtonValue(MasterSystemControllerButton::buttonB) << 3) +
            (resetButton << 4)
            );
}