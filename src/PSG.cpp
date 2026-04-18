#include "PSG.h"
#include "Utils.h"
#include <cmath>

PSG::PSG(SoundConfig *soundConfig) {

    buffer.reserve(2048);

    // 1. Define your source (The PSG) and destination (The Speakers)
    SDL_AudioSpec srcSpec = SDL_AudioSpec ({ SDL_AUDIO_S16LE, 1, 223721 });
    SDL_AudioSpec dstSpec = SDL_AudioSpec ({ SDL_AUDIO_S16LE, 1, 44100 });


    // 2. Create the stream
    audioStream = SDL_CreateAudioStream(&srcSpec, &dstSpec);

    audioDeviceID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    if (audioDeviceID == 0) {
        SDL_Log("SDL could not get audio devices! SDL_Error: %s", SDL_GetError());
    }

    // 3. Bind to the opened device
    if (!SDL_BindAudioStream(audioDeviceID, audioStream)) {
        SDL_Log("SDL could not initialize audio! SDL_Error: %s", SDL_GetError());
    }

    // 4. Important: Unpause the device
    // SDL3 audio devices start paused by default!
    SDL_ResumeAudioDevice(audioDeviceID);

    // Initialise channels
    channels[PSGChannelIndex::Tone0] = new PSGChannel(false);
    channels[PSGChannelIndex::Tone1] = new PSGChannel(false);
    channels[PSGChannelIndex::Tone2] = new PSGChannel(false);
    channels[PSGChannelIndex::Noise] = new PSGChannel(true);

    // Initialise volume table
    float currentVolume = 8000;

    for (int i = 0; i < 0xF; i++) {
        volumeTable[i] = (short)currentVolume;
        currentVolume *= 0.8;
    }

    volumeTable[0xF] = 0;

    selectedRegister = 0;
    hasSelectedVolumeRegister = false;

    cycles = 0;
    clockInfo = 0;

    bufferUpdateLimit = (((float)PSG_CLOCK_SPEED / (((float)SAMPLE_RATE / (float)BUFFER_SIZE) + 1)) / (float)BUFFER_SIZE);

    currentBufferUpdates = 0;

    this->soundConfig = soundConfig;
}

PSG::~PSG() {
    for (auto &channel : channels) {
        delete(channel);
    }
    SDL_DestroyAudioStream(audioStream);
    SDL_CloseAudioDevice(audioDeviceID);
}

void PSG::execute(float soundCycles) {

    if (!soundConfig->isEnabled()) {
        return;
    }

    const int sampleRate = 16;

    soundCycles /= sampleRate;

    cycles += soundCycles;

    float floor = floorf(cycles);
    currentBufferUpdates += floor ;
    clockInfo += floor;

    cycles -= floor;

    unsigned short bufferValue = 0;

    for (int i = 0; i < 3; i++) {
        bufferValue += emulateTone(floor, i);
    }

//    const float CYCLES_PER_SAMPLE = 3579545.0f / 44100.0f;

    if (currentBufferUpdates < bufferUpdateLimit) {
        return;
    }

    buffer.push_back(bufferValue);

    currentBufferUpdates = bufferUpdateLimit - currentBufferUpdates;

}

void PSG::write(unsigned char data) {
    if (Utils::testBit(7, data)) {
        // Program is trying to select and update a new sound channel
        selectedRegister = (data & 0x60) >> 5;
        hasSelectedVolumeRegister = Utils::testBit(4, data);

        if (hasSelectedVolumeRegister) {
            channels[selectedRegister]->setVolume(data);
        } else {
            channels[selectedRegister]->setFrequencyLower(data);
        }

    } else {
        // Program is trying to update the current sound channel
        if (hasSelectedVolumeRegister) {
            channels[selectedRegister]->setVolume(data);
        } else {
            channels[selectedRegister]->setFrequencyHigher(data);
        }


    }
}

void PSG::endFrame() {
    if (!buffer.empty()) {
        SDL_PutAudioStreamData(audioStream, buffer.data(), (int)(buffer.size() * sizeof(int16_t)));
    }

    buffer.clear();
}

unsigned short PSG::emulateTone(float floor, int channelNumber) {

    PSGChannel* channel = channels[channelNumber];

    if (channel->getFrequency() == 0) {
        return 0;
    }

    channel->counter -= floor;

    if (channel->counter <= 0) {
        channel->counter = channel->getFrequency();
        channel->polarity = -channel->polarity;
    }

    return volumeTable[channel->getVolume()] * channel->polarity;
}
