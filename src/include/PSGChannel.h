//
// Created by Peter Savory on 03/07/2023.
//

#ifndef MasterNostalgia_PSGCHANNEL_H
#define MasterNostalgia_PSGCHANNEL_H

#include <SFML/System.hpp>

class PSGChannel {
public:
    PSGChannel(bool isNoiseChannel);

    unsigned char getVolume();

    unsigned short getFrequency();

    void setVolume(unsigned char value);

    void setFrequencyHigher(unsigned char value);

    void setFrequencyLower(unsigned char value);

    int counter{};

    int polarity;

private:

    unsigned char volume;
    unsigned short frequency;
    bool isNoiseChannel;
};

#endif //MasterNostalgia_PSGCHANNEL_H
