#include "Emulator.h"

Emulator::Emulator() {
    system = nullptr;
}

Emulator::~Emulator() {
    if (system) {
        delete(system);
    }
}

void Emulator::init(const std::string &fileName) {
    // TODO detect ROM type and support multiple consoles if we ever get master system support fully working
    system = new MasterSystem();

    bool romLoadResult = system->init(fileName);

    if (!romLoadResult) {
        // TODO provide some more useful error handling
        throw GeneralException("Failed to load ROM file");
    }


}

void Emulator::run() {
    // TODO create SFML window for video output

    while (true) {
        system->emulateFrame();
    }
}

