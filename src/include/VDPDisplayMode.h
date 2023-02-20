//
// Created by Peter Savory on 15/12/2022.
//

#ifndef MASTALGIA_VDPDISPLAYMODE_H
#define MASTALGIA_VDPDISPLAYMODE_H

#include <vector>

enum class SMSDisplayMode : unsigned char {
    NTSCSmall = 0,
    NTSCMedium = 1,
    NTSCLarge = 2,
    PALSmall = 3,
    PALMedium = 4,
    PALLarge = 5
};

class VDPDisplayModeVCounterJump {
public:

    VDPDisplayModeVCounterJump(unsigned char from, unsigned char to) {
        this->from = from;
        this->to = to;
    }

    unsigned char getFrom() {
        return from;
    }

    unsigned char getTo() {
        return to;
    }

private:
    unsigned char from;
    unsigned char to;
};

class VDPDisplayMode {
public:

    VDPDisplayMode() {
        this->activeDisplayEnd = 0;
        this->inactiveDisplayEnd = 0;
        this->isAvailable = false;
        this->vCounterJumps = {};
    }

    VDPDisplayMode(unsigned char activeDisplayEnd,
                   unsigned char inactiveDisplayEnd,
                   bool isAvailable,
                   std::vector<VDPDisplayModeVCounterJump> vCounterJumps) {
        this->activeDisplayEnd = activeDisplayEnd;
        this->inactiveDisplayEnd = inactiveDisplayEnd;
        this->isAvailable = isAvailable;
        this->vCounterJumps = std::move(vCounterJumps);
    }

    unsigned char getActiveDisplayEnd() {
        return activeDisplayEnd;
    }

    unsigned char getInactiveDisplayEnd() {
        return inactiveDisplayEnd; // TODO is this even needed? they all seem to be 0xFF.
    }

    bool getIsAvailable() {
        return isAvailable;
    }

    const std::vector<VDPDisplayModeVCounterJump>& getVCounterJumps() {
        return vCounterJumps;
    }

    static VDPDisplayMode getDisplayMode(SMSDisplayMode mode);

private:
    unsigned char activeDisplayEnd;

    unsigned char inactiveDisplayEnd;

    bool isAvailable;

    std::vector<VDPDisplayModeVCounterJump> vCounterJumps;
};

#endif //MASTALGIA_VDPDISPLAYMODE_H
