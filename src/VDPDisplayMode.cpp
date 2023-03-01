//
// Created by Peter Savory on 15/12/2022.
//

#include "VDPDisplayMode.h"
#include "Exceptions.h"

VDPDisplayMode VDPDisplayMode::getDisplayMode(SMSDisplayMode mode) {
    switch (mode) {
        case SMSDisplayMode::NTSCSmall:
            return VDPDisplayMode(192, 255, true, {
                VDPDisplayModeVCounterJump(0xDA, 0xE5)
            });
        case SMSDisplayMode::NTSCMedium:
            return VDPDisplayMode(224, 255, true, {
                    VDPDisplayModeVCounterJump(0xEA, 0xE5)
            });
        case SMSDisplayMode::NTSCLarge:
            throw VDPException("NTSC Large display mode is not supported");
        case SMSDisplayMode::PALSmall:
            return VDPDisplayMode(192, 255, true, {
                    VDPDisplayModeVCounterJump(0xF2, 0xBA)
            });
        case SMSDisplayMode::PALMedium:
            return VDPDisplayMode(224, 225, false, {
                    VDPDisplayModeVCounterJump(0xFF, 0x0),
                    VDPDisplayModeVCounterJump(0x02, 0xCA)
            });
        case SMSDisplayMode::PALLarge:
            return VDPDisplayMode(240, 255, false, {
                    VDPDisplayModeVCounterJump(0xFF, 0x0),
                    VDPDisplayModeVCounterJump(0x0A, 0xD2)
            });
        default:
            throw VDPException(Utils::implodeString({"Unhandled video mode"}));
    }
}