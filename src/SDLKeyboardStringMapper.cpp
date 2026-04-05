//
// Created by Peter Savory on 23/06/2023.
//

#include "SDLKeyboardStringMapper.h"
#include "Exceptions.h"

SDLKeyboardStringMap::SDLKeyboardStringMap(std::string name, SDL_Scancode key) {
    this->name = name;
    this->key = key;
}

std::string SDLKeyboardStringMap::getName() {
    return name;
}

SDL_Scancode SDLKeyboardStringMap::getKey() {
    return key;
}

SDL_Scancode SDLKeyboardStringMapper::getKey(std::string name) {
    std::string lcName = Utils::strToLower(name);

    for (auto &keyMapping : map) {

        if (lcName != Utils::strToLower(keyMapping.getName())) {
            continue;
        }

        return keyMapping.getKey();
    }

    throw ConfigurationException(Utils::implodeString({"Unknown keyboard key '", name, "'"}));
}

std::string SDLKeyboardStringMapper::getKeyName(SDL_Scancode key) {

    for (auto &keyMapping : map) {

        if (keyMapping.getKey() != key) {
            continue;
        }

        return keyMapping.getName();

    }

    return "Unknown";
}

SDLKeyboardStringMapper::SDLKeyboardStringMapper() {
    map = {
            SDLKeyboardStringMap("Unknown", SDL_SCANCODE_UNKNOWN),
            SDLKeyboardStringMap("A", SDL_SCANCODE_A),
            SDLKeyboardStringMap("B", SDL_SCANCODE_B),
            SDLKeyboardStringMap("C", SDL_SCANCODE_C),
            SDLKeyboardStringMap("D", SDL_SCANCODE_D),
            SDLKeyboardStringMap("E", SDL_SCANCODE_E),
            SDLKeyboardStringMap("F", SDL_SCANCODE_F),
            SDLKeyboardStringMap("G", SDL_SCANCODE_G),
            SDLKeyboardStringMap("H", SDL_SCANCODE_H),
            SDLKeyboardStringMap("I", SDL_SCANCODE_I),
            SDLKeyboardStringMap("J", SDL_SCANCODE_J),
            SDLKeyboardStringMap("K", SDL_SCANCODE_K),
            SDLKeyboardStringMap("L", SDL_SCANCODE_L),
            SDLKeyboardStringMap("M", SDL_SCANCODE_M),
            SDLKeyboardStringMap("N", SDL_SCANCODE_N),
            SDLKeyboardStringMap("O", SDL_SCANCODE_O),
            SDLKeyboardStringMap("P", SDL_SCANCODE_P),
            SDLKeyboardStringMap("Q", SDL_SCANCODE_Q),
            SDLKeyboardStringMap("R", SDL_SCANCODE_R),
            SDLKeyboardStringMap("S", SDL_SCANCODE_S),
            SDLKeyboardStringMap("T", SDL_SCANCODE_T),
            SDLKeyboardStringMap("U", SDL_SCANCODE_U),
            SDLKeyboardStringMap("V", SDL_SCANCODE_V),
            SDLKeyboardStringMap("W", SDL_SCANCODE_W),
            SDLKeyboardStringMap("X", SDL_SCANCODE_X),
            SDLKeyboardStringMap("Y", SDL_SCANCODE_Y),
            SDLKeyboardStringMap("Z", SDL_SCANCODE_Z),
            SDLKeyboardStringMap("Num0", SDL_SCANCODE_0),
            SDLKeyboardStringMap("Num1", SDL_SCANCODE_1),
            SDLKeyboardStringMap("Num2", SDL_SCANCODE_2),
            SDLKeyboardStringMap("Num3", SDL_SCANCODE_3),
            SDLKeyboardStringMap("Num4", SDL_SCANCODE_4),
            SDLKeyboardStringMap("Num5", SDL_SCANCODE_5),
            SDLKeyboardStringMap("Num6", SDL_SCANCODE_6),
            SDLKeyboardStringMap("Num7", SDL_SCANCODE_7),
            SDLKeyboardStringMap("Num8", SDL_SCANCODE_8),
            SDLKeyboardStringMap("Num9", SDL_SCANCODE_9),
            SDLKeyboardStringMap("Escape", SDL_SCANCODE_ESCAPE),
            SDLKeyboardStringMap("LControl", SDL_SCANCODE_LCTRL),
            SDLKeyboardStringMap("LShift", SDL_SCANCODE_LSHIFT),
            SDLKeyboardStringMap("LAlt", SDL_SCANCODE_LALT),
            SDLKeyboardStringMap("LSystem", SDL_SCANCODE_UNKNOWN),
            SDLKeyboardStringMap("RControl", SDL_SCANCODE_RCTRL),
            SDLKeyboardStringMap("RShift", SDL_SCANCODE_RSHIFT),
            SDLKeyboardStringMap("RAlt", SDL_SCANCODE_RALT),
            SDLKeyboardStringMap("RSystem", SDL_SCANCODE_UNKNOWN),
            SDLKeyboardStringMap("Menu", SDL_SCANCODE_MENU),
            SDLKeyboardStringMap("LBracket", SDL_SCANCODE_LEFTBRACKET),
            SDLKeyboardStringMap("RBracket", SDL_SCANCODE_RIGHTBRACKET),
            SDLKeyboardStringMap("Semicolon", SDL_SCANCODE_SEMICOLON),
            SDLKeyboardStringMap("Comma", SDL_SCANCODE_COMMA),
            SDLKeyboardStringMap("Period", SDL_SCANCODE_PERIOD),
            SDLKeyboardStringMap("Apostrophe", SDL_SCANCODE_APOSTROPHE),
            SDLKeyboardStringMap("Slash", SDL_SCANCODE_SLASH),
            SDLKeyboardStringMap("Backslash", SDL_SCANCODE_BACKSLASH),
            SDLKeyboardStringMap("Tilde", SDL_SCANCODE_NONUSBACKSLASH),
            SDLKeyboardStringMap("Equal", SDL_SCANCODE_EQUALS),
            SDLKeyboardStringMap("Hyphen", SDL_SCANCODE_UNKNOWN),
            SDLKeyboardStringMap("Space", SDL_SCANCODE_SPACE),
            SDLKeyboardStringMap("Enter", SDL_SCANCODE_KP_ENTER),
            SDLKeyboardStringMap("Backspace", SDL_SCANCODE_BACKSPACE),
            SDLKeyboardStringMap("Tab", SDL_SCANCODE_TAB),
            SDLKeyboardStringMap("PageUp", SDL_SCANCODE_PAGEUP),
            SDLKeyboardStringMap("PageDown", SDL_SCANCODE_PAGEDOWN),
            SDLKeyboardStringMap("End", SDL_SCANCODE_END),
            SDLKeyboardStringMap("Home", SDL_SCANCODE_HOME),
            SDLKeyboardStringMap("Insert", SDL_SCANCODE_INSERT),
            SDLKeyboardStringMap("Delete", SDL_SCANCODE_DELETE),
            SDLKeyboardStringMap("Add", SDL_SCANCODE_KP_PLUS),
            SDLKeyboardStringMap("Subtract", SDL_SCANCODE_KP_MINUS),
            SDLKeyboardStringMap("Multiply", SDL_SCANCODE_KP_MULTIPLY),
            SDLKeyboardStringMap("Divide", SDL_SCANCODE_KP_DIVIDE),
            SDLKeyboardStringMap("Left", SDL_SCANCODE_LEFT),
            SDLKeyboardStringMap("Right", SDL_SCANCODE_RIGHT),
            SDLKeyboardStringMap("Up", SDL_SCANCODE_UP),
            SDLKeyboardStringMap("Down", SDL_SCANCODE_DOWN),
            SDLKeyboardStringMap("Numpad0", SDL_SCANCODE_KP_0),
            SDLKeyboardStringMap("Numpad1", SDL_SCANCODE_KP_1),
            SDLKeyboardStringMap("Numpad2", SDL_SCANCODE_KP_2),
            SDLKeyboardStringMap("Numpad3", SDL_SCANCODE_KP_3),
            SDLKeyboardStringMap("Numpad4", SDL_SCANCODE_KP_4),
            SDLKeyboardStringMap("Numpad5", SDL_SCANCODE_KP_5),
            SDLKeyboardStringMap("Numpad6", SDL_SCANCODE_KP_6),
            SDLKeyboardStringMap("Numpad7", SDL_SCANCODE_KP_7),
            SDLKeyboardStringMap("Numpad8", SDL_SCANCODE_KP_8),
            SDLKeyboardStringMap("Numpad9", SDL_SCANCODE_KP_9),
            SDLKeyboardStringMap("F1", SDL_SCANCODE_F1),
            SDLKeyboardStringMap("F2", SDL_SCANCODE_F2),
            SDLKeyboardStringMap("F3", SDL_SCANCODE_F3),
            SDLKeyboardStringMap("F4", SDL_SCANCODE_F4),
            SDLKeyboardStringMap("F5", SDL_SCANCODE_F5),
            SDLKeyboardStringMap("F6", SDL_SCANCODE_F6),
            SDLKeyboardStringMap("F7", SDL_SCANCODE_F7),
            SDLKeyboardStringMap("F8", SDL_SCANCODE_F8),
            SDLKeyboardStringMap("F9", SDL_SCANCODE_F9),
            SDLKeyboardStringMap("F10", SDL_SCANCODE_F10),
            SDLKeyboardStringMap("F11", SDL_SCANCODE_F11),
            SDLKeyboardStringMap("F12", SDL_SCANCODE_F12),
            SDLKeyboardStringMap("F13", SDL_SCANCODE_F13),
            SDLKeyboardStringMap("F14", SDL_SCANCODE_F14),
            SDLKeyboardStringMap("F15", SDL_SCANCODE_F15),
            SDLKeyboardStringMap("Pause", SDL_SCANCODE_PAUSE),
            SDLKeyboardStringMap("KeyCount", SDL_SCANCODE_COUNT)
    };
}