/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "MasterSystem.h"

MasterSystem::MasterSystem() {
    smsCartridge = new Cartridge();
    smsMemory = new Memory(smsCartridge);
    smsVdp = new VDP();
    smsPSG = new PSG();
    z80Io = new MasterSystemZ80IO(smsVdp, smsPSG);
    smsCPU = new CPUZ80(smsMemory, z80Io);
    running = false;
}

MasterSystem::~MasterSystem() {
    delete(smsCartridge);
    delete(smsMemory);
    delete(smsCPU);
    delete(smsVdp);
    delete(smsPSG);
    delete(z80Io);
}

bool MasterSystem::init(std::string romFilename) {

    running = true;
    // Load a ROM into memory
    if (!smsCartridge->load(romFilename)) {
        std::cout<<"Error: Unable to load ROM - exiting."<<std::endl;
        // TODO: Send some kind of message to the UI (When it exists) to display an error
        return false;
    }

    return true;
}

unsigned int MasterSystem::tick() {

    int z80ClockCycles = smsCPU->execute();

    unsigned int machineClicks = z80ClockCycles * 3;

    float vdpCycles = machineClicks / 2;

    int soundCycles = z80ClockCycles;

    smsVdp->execute(vdpCycles);

    smsPSG->execute(soundCycles);

    return machineClicks;
}

bool MasterSystem::isRunning() {
    return running && smsCPU->getState() != CPUState::Error && smsCPU->getState() != CPUState::Halt;
}

double MasterSystem::getMachineClicksPerFrame() {
    return (float)10738580 / 60; // TODO this will need to differ for PAL vs. NTSC
}

sf::Uint8* MasterSystem::getVideoOutput() {
    return smsVdp->getVideoOutput();
}