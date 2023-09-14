//
// Created by Peter Savory on 27/10/2022.
//

#ifndef MasterNostalgia_CONSOLE_H
#define MasterNostalgia_CONSOLE_H

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include "Config.h"

class Console {
public:
    virtual bool init(std::string romFilename) = 0;

    void emulateFrame(bool hasFocus) {

        if (hasFocus) {
            storeUserInput(); // TODO should this happen more often than once per frame?
        }

        double machineClicksPerFrame = getMachineClicksPerFrame() * 2; // TODO determine why the machine runs way too slow without this multiplication - might need to revamp how timing works
        double currentClicks = 0;

        while (currentClicks < machineClicksPerFrame) {
            currentClicks += tick();
        }

        handleSaveStorage();
    };

    virtual bool isRunning() = 0;

    virtual double tick() = 0;

    virtual sf::Uint8* getVideoOutput() = 0;

    virtual void storeUserInput() = 0;

    virtual void printVDPInformation() = 0;

    virtual unsigned short getCurrentDisplayWidth() = 0;

    virtual unsigned short getCurrentDisplayHeight() = 0;

    virtual unsigned char getCurrentFrameRate() = 0;

    virtual void sendPauseInterrupt() = 0;

    virtual void shutdown() = 0;

protected:

    virtual double getMachineClicksPerFrame() = 0;

    virtual void handleSaveStorage() = 0;
};

#endif //MasterNostalgia_CONSOLE_H
