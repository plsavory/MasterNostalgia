#include <iostream>
#include "MasterSystem.h"

MasterSystem::MasterSystem(InputInterface *inputInterface, Config *config) {
    smsCartridge = new Cartridge();
    smsMemory = new Memory(smsCartridge);
    smsVdp = new VDP();
    smsPSG = new PSG(config->getSoundConfig());
    smsInput = new MasterSystemInput(inputInterface);
    z80Io = new MasterSystemZ80IO(smsVdp, smsPSG, smsMemory, smsInput);
    smsCPU = new CPUZ80(smsMemory, z80Io);
    running = false;
    this->config = config;
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

    smsVdp->setPALMode(config->getPALOutputMode());

    return true;
}

double MasterSystem::tick() {

    // TODO - the way that timing works needs to be revamped here, it doesn't seem quite right.
    int z80ClockCycles = smsCPU->execute();

    double machineClicks = z80ClockCycles * 3;

    smsVdp->execute(machineClicks / 2);

    smsPSG->execute(z80ClockCycles);

    return machineClicks;
}

bool MasterSystem::isRunning() {
    return running && smsCPU->getState() != CPUState::Error && smsCPU->getState() != CPUState::Halt;
}

double MasterSystem::getMachineClicksPerFrame() {
    return (float)10738580 / (float)(config->getPALOutputMode() ? 50 : 60);
}

sf::Uint8* MasterSystem::getVideoOutput() {
    return smsVdp->getVideoOutput();
}

void MasterSystem::storeUserInput() {
    smsInput->setState();
}

void MasterSystem::printVDPInformation() {
    smsVdp->printDebugInfo();
}

unsigned short MasterSystem::getCurrentDisplayWidth() {
    return 256;
}

unsigned short MasterSystem::getCurrentDisplayHeight() {
    return smsVdp->getDisplayMode().getActiveDisplayEnd();
}

unsigned char MasterSystem::getCurrentFrameRate() {
    return 60; // TODO return 50 if in PAL mode
}

void MasterSystem::sendPauseInterrupt() {
    smsCPU->raisePauseInterrupt();
}