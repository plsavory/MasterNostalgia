#include "PSG.h"
#include "Utils.h"
#include <cmath>

PSG::PSG(SoundConfig *soundConfig) {

    outputBuffer = new sf::SoundBuffer();
    outputSound = new sf::Sound();
    outputSound->setBuffer(*outputBuffer);
    outputSound->setVolume((float)soundConfig->getVolume());

    // Initialise channels
    channels[PSGChannelIndex::Tone0] = new PSGChannel(false);
    channels[PSGChannelIndex::Tone1] = new PSGChannel(false);
    channels[PSGChannelIndex::Tone2] = new PSGChannel(false);
    channels[PSGChannelIndex::Noise] = new PSGChannel(true);

    // Initialise volume table
    float currentVolume = 8000;

    for (int i = 0; i < 0xF; i++) {
        volumeTable[i] = (unsigned short)currentVolume;
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

    delete(outputSound);
    delete(outputBuffer);
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

    if (currentBufferUpdates < bufferUpdateLimit) {
        return;
    }

    if (currentBufferLocation >= BUFFER_SIZE) {
        playBuffer();
    }

    buffer[currentBufferLocation] = bufferValue;

    ++currentBufferLocation;
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

void PSG::playBuffer() {
    outputBuffer->loadFromSamples(buffer, BUFFER_SIZE, 1, SAMPLE_RATE);
    outputSound->setBuffer(*outputBuffer);
    outputSound->play();
    currentBufferLocation = 0;
}

void PSG::clearBuffer() {
    for(int i = 0; i < BUFFER_SIZE; i++) {
        buffer[i] = 0;
    }
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

PSGSaveStateData* PSG::getSaveStateData() {
    auto *result = new PSGSaveStateData();
    result->selectedRegister = selectedRegister;
    result->hasSelectedVolumeRegister = hasSelectedVolumeRegister;

    for(int i = 0; i < 4; i++) {
        result->channelFrequencies[i] = channels[i]->getFrequency();
        result->channelVolumes[i] = channels[i]->getVolume();
    }
    return result;
}

void PSG::restoreState(PSGSaveStateData *data) {

    selectedRegister = data->selectedRegister;
    hasSelectedVolumeRegister = data->hasSelectedVolumeRegister;

    for (int i = 0; i < 4; i++) {
        channels[i]->setFrequencyWhole(data->channelFrequencies[i]);
        channels[i]->setVolume(data->channelVolumes[i]);
    }
}