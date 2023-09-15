#ifndef MASTALGIA_SMSSAVESTATESTRUCTS_H
#define MASTALGIA_SMSSAVESTATESTRUCTS_H

#include <SFML/Window.hpp>

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
    SaveStateKeyPairing(sf::Keyboard::Key key, int id) {
        this->key = key;
        this->id = id;
    }

    sf::Keyboard::Key getKey() {
        return key;
    }

    int getId() {
        return id;
    }

    static std::vector<SaveStateKeyPairing> getKeyPairings() {
        return {
                SaveStateKeyPairing(sf::Keyboard::Key::Num1, 0),
                SaveStateKeyPairing(sf::Keyboard::Key::Num2, 1),
                SaveStateKeyPairing(sf::Keyboard::Key::Num3, 2),
                SaveStateKeyPairing(sf::Keyboard::Key::Num4, 3),
                SaveStateKeyPairing(sf::Keyboard::Key::Num5, 4),
                SaveStateKeyPairing(sf::Keyboard::Key::Num6, 5),
                SaveStateKeyPairing(sf::Keyboard::Key::Num7, 6),
                SaveStateKeyPairing(sf::Keyboard::Key::Num8, 7),
                SaveStateKeyPairing(sf::Keyboard::Key::Num9, 8),
                SaveStateKeyPairing(sf::Keyboard::Key::Num0, 9)
        };
    }

private:
    sf::Keyboard::Key key;
    int id;

};

#endif //MASTALGIA_SMSSAVESTATESTRUCTS_H
