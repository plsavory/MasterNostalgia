//
// Created by Peter Savory on 23/06/2023.
//

#include "SFMLKeyboardStringMapper.h"
#include "Exceptions.h"

SFMLKeyboardStringMap::SFMLKeyboardStringMap(std::string name, SDL_Scancode key) {
    this->name = name;
    this->key = key;
}

std::string SFMLKeyboardStringMap::getName() {
    return name;
}

SDL_Scancode SFMLKeyboardStringMap::getKey() {
    return key;
}

SDL_Scancode SFMLKeyboardStringMapper::getKey(std::string name) {
    std::string lcName = Utils::strToLower(name);

    for (auto &keyMapping : map) {

        if (lcName != Utils::strToLower(keyMapping.getName())) {
            continue;
        }

        return keyMapping.getKey();
    }

    throw ConfigurationException(Utils::implodeString({"Unknown keyboard key '", name, "'"}));
}

std::string SFMLKeyboardStringMapper::getKeyName(SDL_Scancode key) {

    for (auto &keyMapping : map) {

        if (keyMapping.getKey() != key) {
            continue;
        }

        return keyMapping.getName();

    }

    return "Unknown";
}

SFMLKeyboardStringMapper::SFMLKeyboardStringMapper() {
    map = {
            SFMLKeyboardStringMap("Unknown", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("A", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("B", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("C", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("D", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("E", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("G", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("H", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("I", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("J", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("K", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("L", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("M", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("N", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("O", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("P", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Q", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("R", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("S", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("T", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("U", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("V", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("W", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("X", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Y", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Z", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num0", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num1", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num2", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num3", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num4", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num5", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num6", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num7", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num8", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Num9", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Escape", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("LControl", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("LShift", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("LAlt", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("LSystem", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("RControl", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("RShift", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("RAlt", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("RSystem", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Menu", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("LBracket", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("RBracket", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Semicolon", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Comma", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Period", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Apostrophe", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Slash", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Backslash", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Tilde", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Equal", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Hyphen", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Space", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Enter", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Backspace", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Tab", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("PageUp", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("PageDown", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("End", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Home", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Insert", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Delete", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Add", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Subtract", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Multiply", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Divide", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Left", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Right", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Up", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Down", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad0", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad1", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad2", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad3", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad4", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad5", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad6", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad7", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad8", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Numpad9", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F1", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F2", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F3", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F4", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F5", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F6", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F7", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F8", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F9", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F10", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F11", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F12", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F13", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F14", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("F15", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Pause", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("KeyCount", SDL_SCANCODE_UNKNOWN)
    };
}