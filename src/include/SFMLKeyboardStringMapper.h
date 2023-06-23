//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MASTALGIA_SFMLKEYBOARDSTRINGMAPPER_H
#define MASTALGIA_SFMLKEYBOARDSTRINGMAPPER_H

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

#endif //MASTALGIA_SFMLKEYBOARDSTRINGMAPPER_H
