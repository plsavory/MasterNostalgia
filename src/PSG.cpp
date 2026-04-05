#include "PSG.h"
#include "Utils.h"
#include <cmath>
#include <climits>
#include <algorithm>

PSG::PSG(Config *config) {

    buffer.reserve(2048);

    setClockSpeed(config->getPALOutputMode());

    // 1. Define your source (The PSG) and destination (The Speakers)
//    SDL_AudioSpec srcSpec = SDL_AudioSpec ({ SDL_AUDIO_S16LE, 1, PSG_CLOCK_SPEED });
    SDL_AudioSpec srcSpec = SDL_AudioSpec ({ SDL_AUDIO_S16LE, 1, soundClockSpeed });
    SDL_AudioSpec dstSpec = SDL_AudioSpec ({ SDL_AUDIO_S16LE, 1, OUTPUT_FREQUENCY });


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

    // Initialise volume table.
    float currentVolume = 6000.0f;

    for (int i = 0; i < 0xF; i++) {
        volumeTable[i] = (int16_t)currentVolume;
        currentVolume *= 0.7943f; // 10^(-2/20) — accurate 2 dB attenuation per step
    }

    volumeTable[0xF] = 0;

    selectedRegister = 0;
    hasSelectedVolumeRegister = false;

    cycles = 0;
    clockInfo = 0;

    bufferUpdateLimit = (((float)soundClockSpeed / (((float)soundClockSpeed / (float)BUFFER_SIZE) + 1)) / (float)BUFFER_SIZE);

    currentBufferUpdates = 0;


    this->soundConfig = config->getSoundConfig();
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

    int mixedValue = 0;

    for (int i = 0; i < 3; i++) {
        mixedValue += emulateTone(floor, i);
    }

    if (currentBufferUpdates < bufferUpdateLimit) {
        return;
    }

    // Clamp before narrowing to int16_t to guard against any future channel
    // additions (e.g. noise) pushing the sum past the int16_t limits.
    mixedValue = std::max((int)INT16_MIN, std::min((int)INT16_MAX, mixedValue));
    buffer.push_back((int16_t)mixedValue);

    currentBufferUpdates -= bufferUpdateLimit;

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
    // size_t samplesThisFrame = buffer.size(); // DEBUG

    if (!buffer.empty()) {
        SDL_PutAudioStreamData(audioStream, buffer.data(), (int)(buffer.size() * sizeof(int16_t)));
    }

    buffer.clear();

    int queuedBytes = SDL_GetAudioStreamQueued(audioStream);
    double queuedMs = (queuedBytes / (double)sizeof(int16_t)) / (double)soundClockSpeed * 1000.0;

    double errorMs = queuedMs - TARGET_QUEUED_MS;
    double adjustment = std::max(-MAX_RATE_ADJUSTMENT, std::min(MAX_RATE_ADJUSTMENT, errorMs * RATE_CORRECTION_GAIN));
    SDL_SetAudioStreamFrequencyRatio(audioStream, (float)(1.0 + adjustment));

    // DEBUG: log per-frame sample count and the SDL audio device backlog to
    // verify the rate correction keeps the backlog near target.
    // debugFrameCounter++;
    // SDL_Log("[audio debug] frame=%lu samples=%zu queued=%d bytes (~%.1f ms backlog) ratio=%.4f", debugFrameCounter, samplesThisFrame, queuedBytes, queuedMs, 1.0 + adjustment);
}

int16_t PSG::emulateTone(float floor, int channelNumber) {

    PSGChannel* channel = channels[channelNumber];

    if (channel->getFrequency() == 0) {
        return 0;
    }

    channel->counter -= floor;

    if (channel->counter <= 0) {
        channel->counter = channel->getFrequency();
        channel->polarity = -channel->polarity;
    }

    // volumeTable entries are int16_t; polarity is +1 or -1.
    // The product is computed as int and the result always fits in int16_t
    // because abs(volumeTable[vol]) <= 6000.
    return (int16_t)(volumeTable[channel->getVolume()] * channel->polarity);
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

void PSG::setClockSpeed(bool pal) {
    if (pal) {
        soundClockSpeed = PSG_CLOCK_SPEED_PAL;
        return;
    }

    soundClockSpeed = PSG_CLOCK_SPEED;
}