//
// Created by Peter Savory on 23/06/2023.
//

#ifdef JSON_CONFIG_FILE
#include "JsonHandler.hpp"
#include "SDLKeyboardStringMapper.h"
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
    keyBinds[Actions::dPadUp] = SDL_SCANCODE_UP;
    keyBinds[Actions::dPadDown] = SDL_SCANCODE_DOWN;
    keyBinds[Actions::dPadLeft] = SDL_SCANCODE_LEFT;
    keyBinds[Actions::dPadRight] = SDL_SCANCODE_RIGHT;
    keyBinds[Actions::a] = SDL_SCANCODE_A;
    keyBinds[Actions::b] = SDL_SCANCODE_S;
}

SDL_Scancode PlayerControlKeyboardConfig::getBind(int id) {
    return keyBinds[id];
}

#ifdef JSON_CONFIG_FILE

json PlayerControlKeyboardConfig::getJson() {

    SDLKeyboardStringMapper *keyboardMapper = new SDLKeyboardStringMapper();

    json output;

    for (int i = 0; i < 6; i++) {
        output[actionKeys[i]] = keyboardMapper->getKeyName(keyBinds[i]);
    }

    return output;
}

void PlayerControlKeyboardConfig::setFromConfig(json keyboardConfiguration) {
    SDLKeyboardStringMapper *keyboardMapper = new SDLKeyboardStringMapper();


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