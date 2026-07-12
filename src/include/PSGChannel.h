//
// Created by Peter Savory on 03/07/2023.
//

#ifndef MasterNostalgia_PSGCHANNEL_H
#define MasterNostalgia_PSGCHANNEL_H



class PSGChannel {
public:
    PSGChannel(bool isNoiseChannel);

    unsigned char getVolume();

    unsigned short getFrequency();

    void setVolume(unsigned char value);

    void setFrequencyHigher(unsigned char value);

    void setFrequencyLower(unsigned char value);

    void setFrequencyWhole(unsigned short value);

    bool getNoiseWhiteMode();

    unsigned char getNoiseShiftRate();

    void resetLFSR();

    int counter{};

    int polarity;

    unsigned short lfsr{}; // public, following the same pattern as counter/polarity

private:

    unsigned char volume;
    unsigned short frequency;
    bool isNoiseChannel;
};

#endif //MasterNostalgia_PSGCHANNEL_H
