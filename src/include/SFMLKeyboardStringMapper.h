//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H
#define MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H

#include <SDL3/SDL.h>
#include <iostream>

struct SFMLKeyboardStringMap {
public:

    SFMLKeyboardStringMap(std::string name, SDL_Scancode key);

    std::string getName();

    SDL_Scancode getKey();

private:
    std::string name;
    SDL_Scancode key;
};

class SFMLKeyboardStringMapper {
public:

    SFMLKeyboardStringMapper();

    SDL_Scancode getKey(std::string name);

    std::string getKeyName(SDL_Scancode key);

private:
    std::vector<SFMLKeyboardStringMap> map;
};

#endif //MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H
