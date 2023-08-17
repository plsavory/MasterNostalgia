#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "VDP.h"
#include "PSG.h"
#include "InputInterface.h"
#include "Console.h"
#include "MasterSystemInput.h"
#include "MasterSystemZ80IO.h"

class MasterSystem final : public Console {
public:
    MasterSystem(InputInterface *inputInterface, Config *config);

    ~MasterSystem();

    bool init(std::string romFilename) final;

    double tick();

    bool isRunning() final;

    sf::Uint8* getVideoOutput() final;

    void storeUserInput() final;

    void printVDPInformation() final;

    unsigned short getCurrentDisplayWidth() final;

    unsigned short getCurrentDisplayHeight() final;

    unsigned char getCurrentFrameRate() final;

    void sendPauseInterrupt() final;

private:
    CPUZ80 *smsCPU;
    Memory *smsMemory;
    Cartridge *smsCartridge;
    VDP *smsVdp;
    PSG *smsPSG;
    MasterSystemInput *smsInput;
    MasterSystemZ80IO *z80Io;
    Config *config;
    bool running;
protected:

    double getMachineClicksPerFrame() final;
};
