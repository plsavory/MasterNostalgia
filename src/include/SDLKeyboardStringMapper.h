//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MasterNostalgia_SDLKEYBOARDSTRINGMAPPER_H
#define MasterNostalgia_SDLKEYBOARDSTRINGMAPPER_H

#include <SDL3/SDL.h>
#include <vector>
#include <iostream>

struct SDLKeyboardStringMap {
public:

    SDLKeyboardStringMap(std::string name, SDL_Scancode key);

    std::string getName();

    SDL_Scancode getKey();

private:
    std::string name;
    SDL_Scancode key;
};

class SDLKeyboardStringMapper {
public:

    SDLKeyboardStringMapper();

    SDL_Scancode getKey(std::string name);

    std::string getKeyName(SDL_Scancode key);

private:
    std::vector<SDLKeyboardStringMap> map;
};

#endif //MasterNostalgia_SDLKEYBOARDSTRINGMAPPER_H
