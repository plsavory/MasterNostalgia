//
// Created by Peter Savory on 23/06/2023.
//

#include "SFMLKeyboardStringMapper.h"
#include "Exceptions.h"

SFMLKeyboardStringMap::SFMLKeyboardStringMap(std::string name, sf::Keyboard::Key key) {
    this->name = name;
    this->key = key;
}

std::string SFMLKeyboardStringMap::getName() {
    return name;
}

sf::Keyboard::Key SFMLKeyboardStringMap::getKey() {
    return key;
}

sf::Keyboard::Key SFMLKeyboardStringMapper::getKey(std::string name) {
    std::string lcName = Utils::strToLower(name);

    for (auto &keyMapping : map) {

        if (lcName != Utils::strToLower(keyMapping.getName())) {
            continue;
        }

        return keyMapping.getKey();
    }

    throw ConfigurationException(Utils::implodeString({"Unknown keyboard key '", name, "'"}));
}

std::string SFMLKeyboardStringMapper::getKeyName(sf::Keyboard::Key key) {

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
            SFMLKeyboardStringMap("Unknown", sf::Keyboard::Unknown),
            SFMLKeyboardStringMap("A", sf::Keyboard::A),
            SFMLKeyboardStringMap("B", sf::Keyboard::B),
            SFMLKeyboardStringMap("C", sf::Keyboard::C),
            SFMLKeyboardStringMap("D", sf::Keyboard::D),
            SFMLKeyboardStringMap("E", sf::Keyboard::E),
            SFMLKeyboardStringMap("F", sf::Keyboard::F),
            SFMLKeyboardStringMap("G", sf::Keyboard::G),
            SFMLKeyboardStringMap("H", sf::Keyboard::H),
            SFMLKeyboardStringMap("I", sf::Keyboard::I),
            SFMLKeyboardStringMap("J", sf::Keyboard::J),
            SFMLKeyboardStringMap("K", sf::Keyboard::K),
            SFMLKeyboardStringMap("L", sf::Keyboard::L),
            SFMLKeyboardStringMap("M", sf::Keyboard::M),
            SFMLKeyboardStringMap("N", sf::Keyboard::N),
            SFMLKeyboardStringMap("O", sf::Keyboard::O),
            SFMLKeyboardStringMap("P", sf::Keyboard::P),
            SFMLKeyboardStringMap("Q", sf::Keyboard::Q),
            SFMLKeyboardStringMap("R", sf::Keyboard::R),
            SFMLKeyboardStringMap("S", sf::Keyboard::S),
            SFMLKeyboardStringMap("T", sf::Keyboard::T),
            SFMLKeyboardStringMap("U", sf::Keyboard::U),
            SFMLKeyboardStringMap("V", sf::Keyboard::V),
            SFMLKeyboardStringMap("W", sf::Keyboard::W),
            SFMLKeyboardStringMap("X", sf::Keyboard::X),
            SFMLKeyboardStringMap("Y", sf::Keyboard::Y),
            SFMLKeyboardStringMap("Z", sf::Keyboard::Z),
            SFMLKeyboardStringMap("Num0", sf::Keyboard::Num0),
            SFMLKeyboardStringMap("Num1", sf::Keyboard::Num1),
            SFMLKeyboardStringMap("Num2", sf::Keyboard::Num2),
            SFMLKeyboardStringMap("Num3", sf::Keyboard::Num3),
            SFMLKeyboardStringMap("Num4", sf::Keyboard::Num4),
            SFMLKeyboardStringMap("Num5", sf::Keyboard::Num5),
            SFMLKeyboardStringMap("Num6", sf::Keyboard::Num6),
            SFMLKeyboardStringMap("Num7", sf::Keyboard::Num7),
            SFMLKeyboardStringMap("Num8", sf::Keyboard::Num8),
            SFMLKeyboardStringMap("Num9", sf::Keyboard::Num9),
            SFMLKeyboardStringMap("Escape", sf::Keyboard::Escape),
            SFMLKeyboardStringMap("LControl", sf::Keyboard::LControl),
            SFMLKeyboardStringMap("LShift", sf::Keyboard::LShift),
            SFMLKeyboardStringMap("LAlt", sf::Keyboard::LAlt),
            SFMLKeyboardStringMap("LSystem", sf::Keyboard::LSystem),
            SFMLKeyboardStringMap("RControl", sf::Keyboard::RControl),
            SFMLKeyboardStringMap("RShift", sf::Keyboard::RShift),
            SFMLKeyboardStringMap("RAlt", sf::Keyboard::RAlt),
            SFMLKeyboardStringMap("RSystem", sf::Keyboard::RSystem),
            SFMLKeyboardStringMap("Menu", sf::Keyboard::Menu),
            SFMLKeyboardStringMap("LBracket", sf::Keyboard::LBracket),
            SFMLKeyboardStringMap("RBracket", sf::Keyboard::RBracket),
            SFMLKeyboardStringMap("Semicolon", sf::Keyboard::Semicolon),
            SFMLKeyboardStringMap("Comma", sf::Keyboard::Comma),
            SFMLKeyboardStringMap("Period", sf::Keyboard::Period),
            SFMLKeyboardStringMap("Apostrophe", sf::Keyboard::Quote),
            SFMLKeyboardStringMap("Slash", sf::Keyboard::Slash),
            SFMLKeyboardStringMap("Backslash", sf::Keyboard::Backslash),
            SFMLKeyboardStringMap("Tilde", sf::Keyboard::Tilde),
            SFMLKeyboardStringMap("Equal", sf::Keyboard::Equal),
            SFMLKeyboardStringMap("Hyphen", sf::Keyboard::Hyphen),
            SFMLKeyboardStringMap("Space", sf::Keyboard::Space),
            SFMLKeyboardStringMap("Enter", sf::Keyboard::Enter),
            SFMLKeyboardStringMap("Backspace", sf::Keyboard::Backspace),
            SFMLKeyboardStringMap("Tab", sf::Keyboard::Tab),
            SFMLKeyboardStringMap("PageUp", sf::Keyboard::PageUp),
            SFMLKeyboardStringMap("PageDown", sf::Keyboard::PageDown),
            SFMLKeyboardStringMap("End", sf::Keyboard::End),
            SFMLKeyboardStringMap("Home", sf::Keyboard::Home),
            SFMLKeyboardStringMap("Insert", sf::Keyboard::Insert),
            SFMLKeyboardStringMap("Delete", sf::Keyboard::Delete),
            SFMLKeyboardStringMap("Add", sf::Keyboard::Add),
            SFMLKeyboardStringMap("Subtract", sf::Keyboard::Subtract),
            SFMLKeyboardStringMap("Multiply", sf::Keyboard::Multiply),
            SFMLKeyboardStringMap("Divide", sf::Keyboard::Divide),
            SFMLKeyboardStringMap("Left", sf::Keyboard::Left),
            SFMLKeyboardStringMap("Right", sf::Keyboard::Right),
            SFMLKeyboardStringMap("Up", sf::Keyboard::Up),
            SFMLKeyboardStringMap("Down", sf::Keyboard::Down),
            SFMLKeyboardStringMap("Numpad0", sf::Keyboard::Numpad0),
            SFMLKeyboardStringMap("Numpad1", sf::Keyboard::Numpad1),
            SFMLKeyboardStringMap("Numpad2", sf::Keyboard::Numpad2),
            SFMLKeyboardStringMap("Numpad3", sf::Keyboard::Numpad3),
            SFMLKeyboardStringMap("Numpad4", sf::Keyboard::Numpad4),
            SFMLKeyboardStringMap("Numpad5", sf::Keyboard::Numpad5),
            SFMLKeyboardStringMap("Numpad6", sf::Keyboard::Numpad6),
            SFMLKeyboardStringMap("Numpad7", sf::Keyboard::Numpad7),
            SFMLKeyboardStringMap("Numpad8", sf::Keyboard::Numpad8),
            SFMLKeyboardStringMap("Numpad9", sf::Keyboard::Numpad9),
            SFMLKeyboardStringMap("F1", sf::Keyboard::F1),
            SFMLKeyboardStringMap("F2", sf::Keyboard::F2),
            SFMLKeyboardStringMap("F3", sf::Keyboard::F3),
            SFMLKeyboardStringMap("F4", sf::Keyboard::F4),
            SFMLKeyboardStringMap("F5", sf::Keyboard::F5),
            SFMLKeyboardStringMap("F6", sf::Keyboard::F6),
            SFMLKeyboardStringMap("F7", sf::Keyboard::F7),
            SFMLKeyboardStringMap("F8", sf::Keyboard::F8),
            SFMLKeyboardStringMap("F9", sf::Keyboard::F9),
            SFMLKeyboardStringMap("F10", sf::Keyboard::F10),
            SFMLKeyboardStringMap("F11", sf::Keyboard::F10),
            SFMLKeyboardStringMap("F12", sf::Keyboard::F12),
            SFMLKeyboardStringMap("F13", sf::Keyboard::F13),
            SFMLKeyboardStringMap("F14", sf::Keyboard::F14),
            SFMLKeyboardStringMap("F15", sf::Keyboard::F15),
            SFMLKeyboardStringMap("Pause", sf::Keyboard::Pause),
            SFMLKeyboardStringMap("KeyCount", sf::Keyboard::KeyCount)
    };
}