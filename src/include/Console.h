//
// Created by Peter Savory on 27/10/2022.
//

#ifndef MASTALGIA_CONSOLE_H
#define MASTALGIA_CONSOLE_H

class Console {
public:
    virtual bool init(std::string romFilename) = 0;

    void emulateFrame() {
        double machineClicksPerFrame = getMachineClicksPerFrame();
        unsigned int currentClicks = 0;

        while (currentClicks < machineClicksPerFrame) {
            currentClicks = tick();
        }
    };

    virtual bool isRunning() = 0;

    virtual unsigned int tick() = 0;

protected:

    virtual double getMachineClicksPerFrame() = 0;
};

#endif //MASTALGIA_CONSOLE_H