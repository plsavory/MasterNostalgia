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
            SFMLKeyboardStringMap("A", SDL_SCANCODE_A),
            SFMLKeyboardStringMap("B", SDL_SCANCODE_B),
            SFMLKeyboardStringMap("C", SDL_SCANCODE_C),
            SFMLKeyboardStringMap("D", SDL_SCANCODE_D),
            SFMLKeyboardStringMap("E", SDL_SCANCODE_E),
            SFMLKeyboardStringMap("F", SDL_SCANCODE_F),
            SFMLKeyboardStringMap("G", SDL_SCANCODE_G),
            SFMLKeyboardStringMap("H", SDL_SCANCODE_H),
            SFMLKeyboardStringMap("I", SDL_SCANCODE_I),
            SFMLKeyboardStringMap("J", SDL_SCANCODE_J),
            SFMLKeyboardStringMap("K", SDL_SCANCODE_K),
            SFMLKeyboardStringMap("L", SDL_SCANCODE_L),
            SFMLKeyboardStringMap("M", SDL_SCANCODE_M),
            SFMLKeyboardStringMap("N", SDL_SCANCODE_N),
            SFMLKeyboardStringMap("O", SDL_SCANCODE_O),
            SFMLKeyboardStringMap("P", SDL_SCANCODE_P),
            SFMLKeyboardStringMap("Q", SDL_SCANCODE_Q),
            SFMLKeyboardStringMap("R", SDL_SCANCODE_R),
            SFMLKeyboardStringMap("S", SDL_SCANCODE_S),
            SFMLKeyboardStringMap("T", SDL_SCANCODE_T),
            SFMLKeyboardStringMap("U", SDL_SCANCODE_U),
            SFMLKeyboardStringMap("V", SDL_SCANCODE_V),
            SFMLKeyboardStringMap("W", SDL_SCANCODE_W),
            SFMLKeyboardStringMap("X", SDL_SCANCODE_X),
            SFMLKeyboardStringMap("Y", SDL_SCANCODE_Y),
            SFMLKeyboardStringMap("Z", SDL_SCANCODE_Z),
            SFMLKeyboardStringMap("Num0", SDL_SCANCODE_0),
            SFMLKeyboardStringMap("Num1", SDL_SCANCODE_1),
            SFMLKeyboardStringMap("Num2", SDL_SCANCODE_2),
            SFMLKeyboardStringMap("Num3", SDL_SCANCODE_3),
            SFMLKeyboardStringMap("Num4", SDL_SCANCODE_4),
            SFMLKeyboardStringMap("Num5", SDL_SCANCODE_5),
            SFMLKeyboardStringMap("Num6", SDL_SCANCODE_6),
            SFMLKeyboardStringMap("Num7", SDL_SCANCODE_7),
            SFMLKeyboardStringMap("Num8", SDL_SCANCODE_8),
            SFMLKeyboardStringMap("Num9", SDL_SCANCODE_9),
            SFMLKeyboardStringMap("Escape", SDL_SCANCODE_ESCAPE),
            SFMLKeyboardStringMap("LControl", SDL_SCANCODE_LCTRL),
            SFMLKeyboardStringMap("LShift", SDL_SCANCODE_LSHIFT),
            SFMLKeyboardStringMap("LAlt", SDL_SCANCODE_LALT),
            SFMLKeyboardStringMap("LSystem", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("RControl", SDL_SCANCODE_RCTRL),
            SFMLKeyboardStringMap("RShift", SDL_SCANCODE_RSHIFT),
            SFMLKeyboardStringMap("RAlt", SDL_SCANCODE_RALT),
            SFMLKeyboardStringMap("RSystem", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Menu", SDL_SCANCODE_MENU),
            SFMLKeyboardStringMap("LBracket", SDL_SCANCODE_LEFTBRACKET),
            SFMLKeyboardStringMap("RBracket", SDL_SCANCODE_RIGHTBRACKET),
            SFMLKeyboardStringMap("Semicolon", SDL_SCANCODE_SEMICOLON),
            SFMLKeyboardStringMap("Comma", SDL_SCANCODE_COMMA),
            SFMLKeyboardStringMap("Period", SDL_SCANCODE_PERIOD),
            SFMLKeyboardStringMap("Apostrophe", SDL_SCANCODE_APOSTROPHE),
            SFMLKeyboardStringMap("Slash", SDL_SCANCODE_SLASH),
            SFMLKeyboardStringMap("Backslash", SDL_SCANCODE_BACKSLASH),
            SFMLKeyboardStringMap("Tilde", SDL_SCANCODE_NONUSBACKSLASH),
            SFMLKeyboardStringMap("Equal", SDL_SCANCODE_EQUALS),
            SFMLKeyboardStringMap("Hyphen", SDL_SCANCODE_UNKNOWN),
            SFMLKeyboardStringMap("Space", SDL_SCANCODE_SPACE),
            SFMLKeyboardStringMap("Enter", SDL_SCANCODE_KP_ENTER),
            SFMLKeyboardStringMap("Backspace", SDL_SCANCODE_BACKSPACE),
            SFMLKeyboardStringMap("Tab", SDL_SCANCODE_TAB),
            SFMLKeyboardStringMap("PageUp", SDL_SCANCODE_PAGEUP),
            SFMLKeyboardStringMap("PageDown", SDL_SCANCODE_PAGEDOWN),
            SFMLKeyboardStringMap("End", SDL_SCANCODE_END),
            SFMLKeyboardStringMap("Home", SDL_SCANCODE_HOME),
            SFMLKeyboardStringMap("Insert", SDL_SCANCODE_INSERT),
            SFMLKeyboardStringMap("Delete", SDL_SCANCODE_DELETE),
            SFMLKeyboardStringMap("Add", SDL_SCANCODE_KP_PLUS),
            SFMLKeyboardStringMap("Subtract", SDL_SCANCODE_KP_MINUS),
            SFMLKeyboardStringMap("Multiply", SDL_SCANCODE_KP_MULTIPLY),
            SFMLKeyboardStringMap("Divide", SDL_SCANCODE_KP_DIVIDE),
            SFMLKeyboardStringMap("Left", SDL_SCANCODE_LEFT),
            SFMLKeyboardStringMap("Right", SDL_SCANCODE_RIGHT),
            SFMLKeyboardStringMap("Up", SDL_SCANCODE_UP),
            SFMLKeyboardStringMap("Down", SDL_SCANCODE_DOWN),
            SFMLKeyboardStringMap("Numpad0", SDL_SCANCODE_KP_0),
            SFMLKeyboardStringMap("Numpad1", SDL_SCANCODE_KP_1),
            SFMLKeyboardStringMap("Numpad2", SDL_SCANCODE_KP_2),
            SFMLKeyboardStringMap("Numpad3", SDL_SCANCODE_KP_3),
            SFMLKeyboardStringMap("Numpad4", SDL_SCANCODE_KP_4),
            SFMLKeyboardStringMap("Numpad5", SDL_SCANCODE_KP_5),
            SFMLKeyboardStringMap("Numpad6", SDL_SCANCODE_KP_6),
            SFMLKeyboardStringMap("Numpad7", SDL_SCANCODE_KP_7),
            SFMLKeyboardStringMap("Numpad8", SDL_SCANCODE_KP_8),
            SFMLKeyboardStringMap("Numpad9", SDL_SCANCODE_KP_9),
            SFMLKeyboardStringMap("F1", SDL_SCANCODE_F1),
            SFMLKeyboardStringMap("F2", SDL_SCANCODE_F2),
            SFMLKeyboardStringMap("F3", SDL_SCANCODE_F3),
            SFMLKeyboardStringMap("F4", SDL_SCANCODE_F4),
            SFMLKeyboardStringMap("F5", SDL_SCANCODE_F5),
            SFMLKeyboardStringMap("F6", SDL_SCANCODE_F6),
            SFMLKeyboardStringMap("F7", SDL_SCANCODE_F7),
            SFMLKeyboardStringMap("F8", SDL_SCANCODE_F8),
            SFMLKeyboardStringMap("F9", SDL_SCANCODE_F9),
            SFMLKeyboardStringMap("F10", SDL_SCANCODE_F10),
            SFMLKeyboardStringMap("F11", SDL_SCANCODE_F11),
            SFMLKeyboardStringMap("F12", SDL_SCANCODE_F12),
            SFMLKeyboardStringMap("F13", SDL_SCANCODE_F13),
            SFMLKeyboardStringMap("F14", SDL_SCANCODE_F14),
            SFMLKeyboardStringMap("F15", SDL_SCANCODE_F15),
            SFMLKeyboardStringMap("Pause", SDL_SCANCODE_PAUSE),
            SFMLKeyboardStringMap("KeyCount", SDL_SCANCODE_COUNT)
    };
}