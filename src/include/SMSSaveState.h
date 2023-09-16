//
// Created by Peter Savory on 14/09/2023.
//

#ifndef MasterNostalgia_SMSSAVESTATE_H
#define MasterNostalgia_SMSSAVESTATE_H

#include <vector>
#include "SMSSaveStateStructs.h"
#include "Config.h"
#include "Cartridge.h"

class SMSSaveState {
public:

    SMSSaveState(unsigned char id,
                 MemorySaveStateData *memoryData,
                 Z80SaveStateData *z80SaveStateData,
                 VDPSaveStateData *vdpSaveStateData,
                 PSGSaveStateData *psgSaveStateData,
                 Config *config,
                 Cartridge *cartridge);

    SMSSaveState(unsigned char id, Config *config, Cartridge *cartridge);

    ~SMSSaveState();

    MemorySaveStateData* getMemoryData();

    Z80SaveStateData* getZ80SaveStateData();

    VDPSaveStateData* getVdpSaveStateData();

    PSGSaveStateData *getPsgSaveStateData();

    std::string getFileName(Config *config, Cartridge *cartridge);

    bool getIsLoaded();

private:

    unsigned char id;

    MemorySaveStateData *memoryData;

    Z80SaveStateData *z80SaveStateData;

    VDPSaveStateData *vdpSaveStateData;

    PSGSaveStateData *psgSaveStateData;

    static std::vector<unsigned char> getFileHeader();

    void writeToFile(const std::string& fileName);

    bool loadFromFile(const std::string& fileName);

    bool isLoaded;

};

#endif //MasterNostalgia_SMSSAVESTATE_H
