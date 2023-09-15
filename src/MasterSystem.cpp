#include <iostream>
#include "MasterSystem.h"

MasterSystem::MasterSystem(InputInterface *inputInterface, Config *config) {
    smsCartridge = new Cartridge();
    smsMemory = new Memory(smsCartridge, config);
    smsVdp = new VDP();
    smsPSG = new PSG(config->getSoundConfig());
    smsInput = new MasterSystemInput(inputInterface);
    z80Io = new MasterSystemZ80IO(smsVdp, smsPSG, smsMemory, smsInput);
    smsCPU = new CPUZ80(smsMemory, z80Io);
    running = false;
    this->config = config;
    saveStateKeyPressed = false;
    loadStateKeyPressed = false;

    for (auto & saveState : saveStates) {
        saveState = nullptr;
    }
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

    smsMemory->init();

    // Load any save state files which have been found
    for (int i = 0; i < 10; i++) {
        saveStates[i] = new SMSSaveState(i, config, smsCartridge);

        if (saveStates[i]->getIsLoaded()) {
            continue;
        }

        delete(saveStates[i]);
        saveStates[i] = nullptr;
    }
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
    return config->getPALOutputMode() ? 50 : 60;
}

void MasterSystem::sendPauseInterrupt() {
    smsCPU->raisePauseInterrupt();
}

void MasterSystem::shutdown() {
    smsMemory->shutdown();
}

void MasterSystem::handleSaveStorage() {
    smsMemory->handleCRAMWriting();
    handleSaveStateSaving();
    handleSaveStateLoading();
}

void MasterSystem::handleSaveStateSaving() {

    // TODO make the key combinations for saving/loading states optional
    // TODO allow save states to persisted in a file
    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
        saveStateKeyPressed = false;
        return;
    }

    int saveStateId = getSaveStateIdFromKeyPress();

    if (saveStateId == -1) {
        saveStateKeyPressed = false;
        return;
    }

    if (saveStateKeyPressed) {
        return;
    }

    saveStateKeyPressed = true;

    if (saveStates[saveStateId] != nullptr) {
        delete(saveStates[saveStateId]);
    }

    saveStates[saveStateId] = new SMSSaveState(saveStateId,
                                     smsMemory->getSaveStateData(),
                                     smsCPU->getSaveStateData(),
                                     smsVdp->getSaveStateData(),
                                     smsPSG->getSaveStateData(),
                                     config,
                                     smsCartridge);
}

void MasterSystem::handleSaveStateLoading() {

    if (!sf::Keyboard::isKeyPressed(sf::Keyboard::LControl)) {
        loadStateKeyPressed = false;
        return;
    }

    int saveStateId = getSaveStateIdFromKeyPress();

    if (saveStateId == -1) {
        loadStateKeyPressed = false;
        return;
    }

    if (loadStateKeyPressed) {
        return;
    }

    loadStateKeyPressed = true;

    if (!saveStates[saveStateId]) {
        return;
    }

    smsMemory->restoreState(saveStates[saveStateId]->getMemoryData());
    smsCPU->restoreState(saveStates[saveStateId]->getZ80SaveStateData());
    smsVdp->restoreState(saveStates[saveStateId]->getVdpSaveStateData());
    smsPSG->restoreState(saveStates[saveStateId]->getPsgSaveStateData());
}

int MasterSystem::getSaveStateIdFromKeyPress() {

    for (auto & datum : SaveStateKeyPairing::getKeyPairings()) {

        if (!sf::Keyboard::isKeyPressed(datum.getKey())) {
            continue;
        }

        return datum.getId();
    }

    return -1;
}