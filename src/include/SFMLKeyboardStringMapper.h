//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H
#define MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H

#include <iostream>

#define SDL_Scancode int // TODO SDL port
#define SDL_SCANCODE_UNKNOWN 0 // TODO SDL port

struct SFMLKeyboardStringMap {
public:

    SFMLKeyboardStringMap(std::string name, SDL_Scancode key);

    std::string getName();

    SDL_Scancode getKey();

private:
    std::string name;
    int key;
};

class SFMLKeyboardStringMapper {
public:

    SFMLKeyboardStringMapper();

    SDL_Scancode getKey(std::string name);

    std::string getKeyName(int key);

private:
    std::vector<SFMLKeyboardStringMap> map;
};

#endif //MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H
