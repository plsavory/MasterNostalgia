//
// Created by Peter Savory on 27/10/2022.
//

#ifndef MASTALGIA_CONSOLE_H
#define MASTALGIA_CONSOLE_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

class Console {
public:
    virtual bool init(std::string romFilename) = 0;

    void emulateFrame() {
        storeUserInput(); // TODO should this happen more often than once per frame?
        double machineClicksPerFrame = getMachineClicksPerFrame();
        unsigned int currentClicks = 0;

        while (currentClicks < machineClicksPerFrame) {
            currentClicks += tick();
        }
    };

    virtual bool isRunning() = 0;

    virtual unsigned int tick() = 0;

    virtual sf::Uint8* getVideoOutput() = 0;

    virtual void storeUserInput() = 0;

    virtual void printVDPInformation() = 0;

    virtual unsigned short getCurrentDisplayWidth() = 0;

    virtual unsigned short getCurrentDisplayHeight() = 0;

    virtual unsigned char getCurrentFrameRate() = 0;

protected:

    virtual double getMachineClicksPerFrame() = 0;
};

#endif //MASTALGIA_CONSOLE_H
