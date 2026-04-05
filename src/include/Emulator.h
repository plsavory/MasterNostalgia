#ifndef MasterNostalgia_EMULATOR_H
#define MasterNostalgia_EMULATOR_H

#include <iostream>
#include "Config.h"
#include "Utils.h"
#include "MasterSystem.h"
#include "Exceptions.h"
#include "InputInterface.h"

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

//    sf::RenderWindow *window; TODO SDL port

    Config *config;

    void setVideoMode(unsigned int width, unsigned int height);

    void setRenderingTexture();

    unsigned short renderWidth;
    unsigned short renderHeight;

//    sf::Texture videoOutputTexture;

    InputInterface *inputInterface;
};

#endif