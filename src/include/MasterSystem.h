/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "VDP.h"
#include "PSG.h"
#include "Console.h"
#include "MasterSystemZ80IO.h"

class MasterSystem : public Console {
public:
    MasterSystem();

    ~MasterSystem();

    bool init(std::string romFilename) final;

    unsigned int tick();

    bool isRunning() final;

    sf::Uint8* getVideoOutput() final;

private:
    CPUZ80 *smsCPU;
    Memory *smsMemory;
    Cartridge *smsCartridge;
    VDP *smsVdp;
    PSG *smsPSG;
    MasterSystemZ80IO *z80Io;
    bool running;
protected:

    double getMachineClicksPerFrame() final;
};
