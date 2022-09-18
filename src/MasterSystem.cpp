/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "MasterSystem.h"

MasterSystem::MasterSystem() {
    // Initialize the Emulator

#ifdef VERBOSE_MODE
    std::cout<<"Initializing Emulator..."<<std::endl;
#endif

    smsCartridge = new Cartridge();
    smsMemory = new Memory(smsCartridge);
    smsCPU = new CPUZ80(smsMemory);
    smsVdp = new VDP();
    smsPSG = new PSG();
    running = false;
}

MasterSystem::~MasterSystem() {

}

bool MasterSystem::start(std::string romFilename) {

    running = true;
    // Load a ROM into memory

    if (!smsCartridge->load(romFilename)) {

#ifdef VERBOSE_MODE
        std::cout<<"Error: Unable to load ROM - exiting."<<std::endl;
#endif

        // TODO: Send some kind of message to the UI (When it exists) to display an error
        return false;

    } else {

#ifdef VERBOSE_MODE
        std::cout<<"ROM Loaded successfully"<<std::endl;
#endif

    };

    // Enter the main loop
    run();

    return true;
}

void MasterSystem::tick() {

    int z80ClockCycles = smsCPU->execute();

    int machineClicks = z80ClockCycles * 3;

    float vdpCycles = machineClicks / 2;

    int soundCycles = z80ClockCycles;

    smsVdp->execute(vdpCycles);

    smsPSG->execute(soundCycles);

}

void MasterSystem::run() {

#ifdef VERBOSE_MODE
    std::cout<<std::endl<<"--- Starting code execution ---"<<std::endl;
#endif

    while (isRunning()) {
        tick();
    }
}

bool MasterSystem::isRunning() {
    return running && smsCPU->getState() != cpuState::Error && smsCPU->getState() != cpuState::Halt;
}