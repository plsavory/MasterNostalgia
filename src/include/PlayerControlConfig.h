//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MASTALGIA_PLAYERCONTROLCONFIG_H
#define MASTALGIA_PLAYERCONTROLCONFIG_H

// TODO should anything SFML related be moved out of this class into an extra layer? Do this later if we need portability between APIs (e.g an SDL version).
#include <SFML/Window.hpp>

class PlayerControlKeyboardConfig {
public:

    PlayerControlKeyboardConfig();

    void setDefaults();

    enum Actions {
        dPadUp = 0,
        dPadDown = 1,
        dPadLeft = 2,
        dPadRight = 3,
        a = 4,
        b = 5
    };

    sf::Keyboard::Key getBind(int id);


#ifdef JSON_CONFIG_FILE

    json getJson();

    void setFromConfig(json keyboardConfiguration);

#endif

private:
    sf::Keyboard::Key keyBinds[6];

#ifdef JSON_CONFIG_FILE
    std::vector<std::string> actionKeys;
#endif

};

class PlayerControlConfig {
public:

    PlayerControlConfig();

    void setDefaults();

    ~PlayerControlConfig();

    PlayerControlKeyboardConfig* getKeyboardConfig();

#ifdef JSON_CONFIG_FILE

    void setFromConfig(json playerControlConfiguration);

    json getJson();

#endif

private:

    PlayerControlKeyboardConfig *keyboardConfig;

};

#endif //MASTALGIA_PLAYERCONTROLCONFIG_H
