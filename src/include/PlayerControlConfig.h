//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MasterNostalgia_PLAYERCONTROLCONFIG_H
#define MasterNostalgia_PLAYERCONTROLCONFIG_H

#define SDL_Scancode int // TODO SDL port
#define SDL_SCANCODE_UNKNOWN 0 // TODO SDL port

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

    SDL_Scancode getBind(int id);


#ifdef JSON_CONFIG_FILE

    json getJson();

    void setFromConfig(json keyboardConfiguration);

#endif

private:
    SDL_Scancode keyBinds[6];

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

#endif //MasterNostalgia_PLAYERCONTROLCONFIG_H
