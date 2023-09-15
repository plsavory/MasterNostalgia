//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H
#define MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H

#include <iostream>
#include <SFML/Window.hpp>

struct SFMLKeyboardStringMap {
public:

    SFMLKeyboardStringMap(std::string name, sf::Keyboard::Key key);

    std::string getName();

    sf::Keyboard::Key getKey();

private:
    std::string name;
    sf::Keyboard::Key key;
};

class SFMLKeyboardStringMapper {
public:

    SFMLKeyboardStringMapper();

    sf::Keyboard::Key getKey(std::string name);

    std::string getKeyName(sf::Keyboard::Key key);

private:
    std::vector<SFMLKeyboardStringMap> map;
};

#endif //MasterNostalgia_SFMLKEYBOARDSTRINGMAPPER_H
