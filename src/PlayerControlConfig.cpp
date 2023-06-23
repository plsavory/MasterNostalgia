//
// Created by Peter Savory on 23/06/2023.
//

#ifdef JSON_CONFIG_FILE
#include "JsonHandler.hpp"
#include "SFMLKeyboardStringMapper.h"
#endif

#include "PlayerControlConfig.h"

PlayerControlConfig::PlayerControlConfig() {
    keyboardConfig = nullptr;
}

PlayerControlConfig::~PlayerControlConfig() {
    delete(keyboardConfig);
}

void PlayerControlConfig::setDefaults() {
    keyboardConfig = new PlayerControlKeyboardConfig();
    keyboardConfig->setDefaults();
}

PlayerControlKeyboardConfig* PlayerControlConfig::getKeyboardConfig() {
    return keyboardConfig;
}

#ifdef JSON_CONFIG_FILE

json PlayerControlConfig::getJson() {
    json output;

    if (keyboardConfig) {
        json keyboardJsonConfiguration = keyboardConfig->getJson();

        if (!keyboardJsonConfiguration.empty()) {
            output["keyboard"] = keyboardConfig->getJson();
        }
    }

    // TODO store the game pad configuration

    return output;
}

void PlayerControlConfig::setFromConfig(json playerControlConfiguration) {

    if (JsonHandler::keyExists(playerControlConfiguration, "keyboard")) {
        keyboardConfig->setFromConfig(playerControlConfiguration["keyboard"]);
    } else {
        delete(keyboardConfig);
        keyboardConfig = nullptr;
    }

    // TODO set up the game pad button configuration
}
#endif

PlayerControlKeyboardConfig::PlayerControlKeyboardConfig() {
#ifdef JSON_CONFIG_FILE
    actionKeys = {
            "dPadUp",
            "dPadDown",
            "dPadLeft",
            "dPadRight",
            "a",
            "b"
    };
#endif
}

void PlayerControlKeyboardConfig::setDefaults() {
    keyBinds[Actions::dPadUp] = sf::Keyboard::Up;
    keyBinds[Actions::dPadDown] = sf::Keyboard::Down;
    keyBinds[Actions::dPadLeft] = sf::Keyboard::Left;
    keyBinds[Actions::dPadRight] = sf::Keyboard::Right;
    keyBinds[Actions::a] = sf::Keyboard::A;
    keyBinds[Actions::b] = sf::Keyboard::S;
}

sf::Keyboard::Key PlayerControlKeyboardConfig::getBind(int id) {
    return keyBinds[id];
}

#ifdef JSON_CONFIG_FILE

json PlayerControlKeyboardConfig::getJson() {

    SFMLKeyboardStringMapper *keyboardMapper = new SFMLKeyboardStringMapper();

    json output;

    for (int i = 0; i < 6; i++) {
        output[actionKeys[i]] = keyboardMapper->getKeyName(keyBinds[i]);
    }

    return output;
}

void PlayerControlKeyboardConfig::setFromConfig(json keyboardConfiguration) {
    SFMLKeyboardStringMapper *keyboardMapper = new SFMLKeyboardStringMapper();


    try {

        for (int i = 0; i < 6; i++) {
            std::string key = actionKeys[i];

            if (!JsonHandler::keyExists(keyboardConfiguration, key)) {
                throw ConfigurationException(Utils::implodeString({"Required key '", key, "' for keyboard configuration is missing"}));
            }

            keyBinds[i] = keyboardMapper->getKey(JsonHandler::getString(keyboardConfiguration, key));

            // TODO allow a keycode to be used
        }
        delete(keyboardMapper);
    } catch (GeneralException &e) {
        delete(keyboardMapper);
        throw e;
    }

}

#endif