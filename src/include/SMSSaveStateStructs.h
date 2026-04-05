#ifndef MasterNostalgia_SMSSAVESTATESTRUCTS_H
#define MasterNostalgia_SMSSAVESTATESTRUCTS_H

#include <SDL3/SDL.h>

struct MemorySaveStateData {
public:

    MemorySaveStateData() = default;

//    void fromFileData(std::vector<unsigned char> data) {
//
//    };
//
//    std::vector<unsigned char> toFileData() {
//        /*
//         * Proposed format:
//         * Bytes 0x0-0x6: Memory Pages (Addresses)
//         * Byte 0x7: Control Register
//         * Bytes 0x8-0x10008: RAM
// */
//        std::vector<unsigned char> result;
//        return result;
//    }

    unsigned char ram[0x10000]{};
    unsigned short memoryPages[3]{};
    unsigned char controlRegister{};

private:
};

struct Z80SaveStateData {
public:
    unsigned short registers[11];
    unsigned char registerI;
    unsigned char registerR;
    unsigned short programCounter;
    unsigned short stackPointer;
    bool iff1;
    bool iff2;
    bool enableInterrupts;
};

struct VDPSaveStateData {
public:
    unsigned char vRAM[0x4000];
    unsigned char cRAM[0x40];
    unsigned char statusRegister;
    unsigned char registers[11];
    unsigned short controlWord;
    unsigned char readBuffer;
    bool requestInterrupt;
    bool isSecondControlWrite;
    unsigned char vScroll;
    unsigned char vCounter;
    unsigned char lineInterruptCounter;
    unsigned char vCounterJumpCount;
};

struct PSGSaveStateData {
public:
    unsigned char selectedRegister;
    bool hasSelectedVolumeRegister;
    unsigned short channelFrequencies[4];
    unsigned char channelVolumes[4];
};

struct SaveStateKeyPairing {
public:
    SaveStateKeyPairing(SDL_Scancode key, int id) {
        this->key = key;
        this->id = id;
    }

    SDL_Scancode getKey() {
        return key;
    }

    int getId() {
        return id;
    }

    static std::vector<SaveStateKeyPairing> getKeyPairings() {
        return {
                SaveStateKeyPairing(SDL_SCANCODE_1, 0),
                SaveStateKeyPairing(SDL_SCANCODE_2, 1),
                SaveStateKeyPairing(SDL_SCANCODE_3, 2),
                SaveStateKeyPairing(SDL_SCANCODE_4, 3),
                SaveStateKeyPairing(SDL_SCANCODE_5, 4),
                SaveStateKeyPairing(SDL_SCANCODE_6, 5),
                SaveStateKeyPairing(SDL_SCANCODE_7, 6),
                SaveStateKeyPairing(SDL_SCANCODE_8, 7),
                SaveStateKeyPairing(SDL_SCANCODE_9, 8),
                SaveStateKeyPairing(SDL_SCANCODE_0, 9)
        };
    }

private:
    SDL_Scancode key;
    int id;

};

#endif //MasterNostalgia_SMSSAVESTATESTRUCTS_H
