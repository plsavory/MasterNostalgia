#ifndef MasterNostalgia_EMULATOR_H
#define MasterNostalgia_EMULATOR_H

#include <iostream>
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
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

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;

    SDL_Texture* m_texture = nullptr;

    Config *config;

    void setRenderingTexture();

    int renderWidth;
    int renderHeight;

    InputInterface *inputInterface;

    void shutdown();
};

#endif