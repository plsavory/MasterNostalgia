#ifndef SMS_PSG_H
#define SMS_PSG_H

#define BUFFER_SIZE 1024

#define OUTPUT_FREQUENCY 44100

// 3.3Mhz / 16
// TODO should this be 223720 ((machine clock/3)/16)? Do a comparison and determine which sounds more correct.
#define PSG_CLOCK_SPEED 223721
#define PSG_CLOCK_SPEED_PAL 221681

#include <SDL3/SDL.h>
#include "PSGChannel.h"
#include "Config.h"
#include "SoundConfig.h"

#include <bitset>
#include "SMSSaveStateStructs.h"

enum PSGChannelIndex {
    Tone0 = 0,
    Tone1 = 1,
    Tone2 = 2,
    Noise = 3
};

class PSG {
public:
    PSG(Config *config);

    ~PSG();

    void execute(float soundCycles);

    void write(unsigned char data);

    PSGSaveStateData* getSaveStateData();

    void restoreState(PSGSaveStateData *data);

    void endFrame();

    void setClockSpeed(bool pal);

private:
    PSGChannel *channels[4];
    int16_t volumeTable[16];
    unsigned char selectedRegister;
    bool hasSelectedVolumeRegister;
    int soundClockSpeed;

    float cycles;
    unsigned long int clockInfo;

    std::vector<int16_t> buffer; // Dynamically sized for one frame

    SDL_AudioStream *audioStream = nullptr;
    SDL_AudioDeviceID audioDeviceID = 0;
    float bufferUpdateLimit ;
    float currentBufferUpdates;

    unsigned long debugFrameCounter = 0;

    // Dynamic rate correction: nudges the audio stream's playback speed by a
    // fraction of a percent to pull the queued backlog back towards a target
    // level, rather than letting a slow production/consumption mismatch
    // accumulate into ever-growing latency.
    static constexpr double TARGET_QUEUED_MS = 40.0;
    static constexpr double RATE_CORRECTION_GAIN = 0.0002;
    static constexpr double MAX_RATE_ADJUSTMENT = 0.005;

    inline bool getParity(unsigned short value) {
        return std::bitset<4>(value).count() % 2 == 0;
    }

    int16_t emulateTone(float floor, int channelNumber);

    int16_t emulateNoise(float floor, PSGChannel *noise, PSGChannel *tone2);

    SoundConfig *soundConfig;

    float getBufferUpdateLimit();

};

#endif