//
// Created by Peter Savory on 26/06/2023.
//

#include "GeneralControlConfig.h"
#include "SDLKeyboardStringMapper.h"

GeneralControlConfig::GeneralControlConfig() {
    keyboard = nullptr;
}

void GeneralControlConfig::setDefaults() {
    keyboard = new GeneralControlConfigKeyboard();
    keyboard->setDefaults();

}

GeneralControlConfigKeyboard *GeneralControlConfig::getKeyboardConfig() {
    return keyboard;
}

#ifdef JSON_CONFIG_FILE

json GeneralControlConfig::getJson() {
    json output;

    if (keyboard != nullptr) {
        output["keyboard"] = keyboard->getJson();
    }

    return output;
}

void GeneralControlConfig::setFromConfig(json generalControlConfiguration) {
    if (JsonHandler::keyExists(generalControlConfiguration, "keyboard")) {
        keyboard = new GeneralControlConfigKeyboard();
        keyboard->setFromConfig(generalControlConfiguration["keyboard"]);
    }
}

void GeneralControlConfigKeyboard::setFromConfig(json generalControlKeyboardConfiguration) {
    SDLKeyboardStringMapper *mapper = new SDLKeyboardStringMapper();

    if (JsonHandler::keyExists(generalControlKeyboardConfiguration, "pause")) {
        pauseKey = mapper->getKey(JsonHandler::getString(generalControlKeyboardConfiguration, "pause"));
    }

    if (JsonHandler::keyExists(generalControlKeyboardConfiguration, "exit")) {
        exitKey = mapper->getKey(JsonHandler::getString(generalControlKeyboardConfiguration, "exit"));
    }

    delete(mapper);
}

json GeneralControlConfigKeyboard::getJson() {
    json output;
    SDLKeyboardStringMapper *mapper = new SDLKeyboardStringMapper();

    output["exit"] = mapper->getKeyName(exitKey);
    output["pause"] = mapper->getKeyName(pauseKey);

    delete (mapper);
    return output;
}

#endif

GeneralControlConfigKeyboard::GeneralControlConfigKeyboard() {
    pauseKey = SDL_SCANCODE_P;
    exitKey = SDL_SCANCODE_ESCAPE;
}

void GeneralControlConfigKeyboard::setDefaults() {
    pauseKey = SDL_SCANCODE_P;
    exitKey = SDL_SCANCODE_ESCAPE;
}

SDL_Scancode GeneralControlConfigKeyboard::getPauseKey() {
    return pauseKey;
}

SDL_Scancode GeneralControlConfigKeyboard::getExitKey() {
    return exitKey;
}