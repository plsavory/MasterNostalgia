//
// Created by Peter Savory on 03/07/2023.
//

#include "PSGChannel.h"

PSGChannel::PSGChannel(bool isNoiseChannel) {
    this->isNoiseChannel = isNoiseChannel;
    volume = 0xF;
    frequency = 0x0;

    polarity = 1;
    counter = 0;
}

unsigned short PSGChannel::getFrequency() {
    return frequency;
}

void PSGChannel::setVolume(unsigned char value) {
    volume = value & 0xF;
}

unsigned char PSGChannel::getVolume() {
    return volume;
}

void PSGChannel::setFrequencyLower(unsigned char value) {

    if (isNoiseChannel) {
        frequency = value & 0xF;
        return;
    }

    frequency = (frequency & 0x3C0) + (value & 0xF);
}

void PSGChannel::setFrequencyHigher(unsigned char value) {
    if (isNoiseChannel) {
        frequency = value & 0xF;
        return;
    }

    frequency = ((value & 0x3F) << 4) + (frequency & 0xF);

}

void PSGChannel::setFrequencyWhole(unsigned short value) {
    frequency = value;
}