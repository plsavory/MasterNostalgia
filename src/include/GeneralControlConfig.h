//
// Created by Peter Savory on 26/06/2023.
//

#ifndef MasterNostalgia_GENERALCONTROLCONFIG_H
#define MasterNostalgia_GENERALCONTROLCONFIG_H

#include <SDL3/SDL.h>

#ifdef JSON_CONFIG_FILE
#include "JsonHandler.hpp"
#endif

class GeneralControlConfigKeyboard {
public:

    GeneralControlConfigKeyboard();

    void setDefaults();

    SDL_Scancode getPauseKey();

    SDL_Scancode getExitKey();

#ifdef JSON_CONFIG_FILE

    void setFromConfig(json generalControlKeyboardConfiguration);


    json getJson();
#endif

private:

    SDL_Scancode exitKey;

    SDL_Scancode pauseKey;

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

#endif //MasterNostalgia_GENERALCONTROLCONFIG_H
