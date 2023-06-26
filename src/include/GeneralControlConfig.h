//
// Created by Peter Savory on 26/06/2023.
//

#ifndef MASTALGIA_GENERALCONTROLCONFIG_H
#define MASTALGIA_GENERALCONTROLCONFIG_H

#include <SFML/Window.hpp>

#ifdef JSON_CONFIG_FILE
#include "JsonHandler.hpp"
#endif

class GeneralControlConfigKeyboard {
public:

    GeneralControlConfigKeyboard();

    void setDefaults();

    sf::Keyboard::Key getPauseKey();

    sf::Keyboard::Key getExitKey();

#ifdef JSON_CONFIG_FILE

    void setFromConfig(json generalControlKeyboardConfiguration);


    json getJson();
#endif

private:

    sf::Keyboard::Key exitKey;

    sf::Keyboard::Key pauseKey;

};

class GeneralControlConfig {
public:

    GeneralControlConfig();

    void setDefaults();

    GeneralControlConfigKeyboard *getKeyboardConfig();

#ifdef JSON_CONFIG_FILE

    void setFromConfig(json generalControlConfiguration);


    json getJson();
#endif

private:

    GeneralControlConfigKeyboard *keyboard;

};

#endif //MASTALGIA_GENERALCONTROLCONFIG_H
