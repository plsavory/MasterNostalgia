#include "PSG.h"
#include "Utils.h"
#include <cmath>
#include <iostream>

PSG::PSG(SoundConfig *soundConfig) {

//    TODO SDL port
//    outputBuffer = new sf::SoundBuffer();
//    outputSound = new sf::Sound();
//    outputSound->setBuffer(*outputBuffer);
//    outputSound->setVolume((float)soundConfig->getVolume());

    buffer.reserve(2048);

    // 1. Define your source (The PSG) and destination (The Speakers)
    srcSpec = new SDL_AudioSpec ({ SDL_AUDIO_S16LE, 1, 223721 });
    dstSpec = new SDL_AudioSpec ({ SDL_AUDIO_S16LE, 1, 44100 });

    // 2. Create the stream
    audioStream = SDL_CreateAudioStream(srcSpec, dstSpec);

    SDL_AudioDeviceID devID = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, dstSpec);
    if (devID == 0) {
        // Log error: SDL_GetError()
        std::cout<<"Unable to get SDL audio device ID"<<std::endl;
    }

    // 3. Bind to the DEFAULT playback device
    // In SDL3, this constant replaces the 'GetDefault' function call
    if (!SDL_BindAudioStream(devID, audioStream)) {
        SDL_Log("SDL could not initialize audio! SDL_Error: %s", SDL_GetError());
    }

    // 4. Important: Unpause the device
    // SDL3 audio devices start paused by default!
    SDL_ResumeAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);

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

    clearBuffer();

    bufferUpdateLimit = ((float)PSG_CLOCK_SPEED / (((float)SAMPLE_RATE / (float)BUFFER_SIZE) + 1)) / (float)BUFFER_SIZE;

    currentBufferUpdates = 0;

    this->soundConfig = soundConfig;
}

PSG::~PSG() {
    for (auto &channel : channels) {
        delete(channel);
    }

//    TODO SDL port
//    delete(outputSound);
//    delete(outputBuffer);
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

    short bufferValue = 0;

    for (int i = 0; i < 3; i++) {
        bufferValue += emulateTone(floor, i);
    }

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
    // 1. Push whatever samples we generated this frame to SDL
    // No 'loadFromSamples' or 'setBuffer' needed.
    if (!buffer.empty()) {
        SDL_PutAudioStreamData(audioStream, buffer.data(), (int)(buffer.size() * sizeof(int16_t)));
    }

//    std::cout<<"Playing buffer, size: "<<buffer.size()<<std::endl;

    // 2. Clear the counter for the next 1/60th of a second
    buffer.clear();
}

void PSG::clearBuffer() {
    // TODO SDL port
//    for(int i = 0; i < BUFFER_SIZE; i++) {
//        buffer[i] = 0;
//    }
}

short PSG::emulateTone(float floor, int channelNumber) {

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
