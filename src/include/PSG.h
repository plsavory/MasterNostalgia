#ifndef SMS_PSG_H
#define SMS_PSG_H

#define BUFFER_SIZE 4096
#define SAMPLE_RATE 44100

// 3.3Mhz / 16
// TODO should this be 223720 ((machine clock/3)/16)? Do a comparison and determine which sounds more correct.
#define PSG_CLOCK_SPEED 220000

// 223720
#include "PSGChannel.h"
#include "SoundConfig.h"
#include <SFML/Audio.hpp>

enum PSGChannelIndex {
    Tone0 = 0,
    Tone1 = 1,
    Tone2 = 2,
    Noise = 3
};

class PSG {
public:
    PSG(SoundConfig *soundConfig);

    ~PSG();

    void execute(float soundCycles);

    void write(unsigned char data);

private:
    PSGChannel *channels[4];
    unsigned short volumeTable[16];
    unsigned char selectedRegister;
    bool hasSelectedVolumeRegister;

    float cycles;
    unsigned long int clockInfo;

    sf::Int16 buffer[BUFFER_SIZE];

    unsigned long currentBufferLocation;

    void playBuffer();
    void clearBuffer();

    sf::SoundBuffer *outputBuffer;
    sf::Sound *outputSound;
    float bufferUpdateLimit ;
    float currentBufferUpdates;

    inline bool getParity(unsigned short value) {
        return std::bitset<4>(value).count() % 2 == 0;
    }

    unsigned short emulateTone(float floor, int channelNumber);

    SoundConfig *soundConfig;
};

#endif