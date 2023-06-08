#ifndef MASTALGIA_EMULATOR_H
#define MASTALGIA_EMULATOR_H

#include <iostream>
#include "Utils.h"
#include "MasterSystem.h"
#include "Exceptions.h"
#include "SFML/System.hpp"
#include "SFML/Graphics.hpp"

/**
 * A class for handling loading/running different systems. Right now only the Master System is supported.
 */
class Emulator {
public:
    Emulator();

    ~Emulator();

    void init(const std::string &fileName);

    void run();

private:
    MasterSystem *system;

    sf::RenderWindow *window;

    void setVideoMode(unsigned int width, unsigned int height);

    void setRenderingTexture();

    unsigned short renderWidth;
    unsigned short renderHeight;

    unsigned short windowWidth;
    unsigned short windowHeight;

    sf::Texture videoOutputTexture;

    sf::Sprite videoOutputSprite;
};

#endif