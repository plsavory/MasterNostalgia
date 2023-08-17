#include "SoundConfig.h"

SoundConfig::SoundConfig() {
    enabled = true;
    volume = 50;
}

#ifdef JSON_CONFIG_FILE
json SoundConfig::getSoundConfigurationJson() {
    json output;
    output["enabled"] = enabled;
    output["volume"] = volume;
    return output;
}

void SoundConfig::setFromConfig(json soundConfigurationJson) {

    if (JsonHandler::keyExists(soundConfigurationJson, "enabled")) {
        enabled = JsonHandler::getBoolean(soundConfigurationJson, "enabled");
    }

    if (JsonHandler::keyExists(soundConfigurationJson, "volume")) {
        volume = JsonHandler::getInteger(soundConfigurationJson, "volume");

        if (volume < 0) {
            volume = 0;
        }

        if (volume > 100) {
            volume = 100;
        }
    }
    // TODO allow for other options such as output buffer size etc.
}

#endif

bool SoundConfig::isEnabled() {
    return enabled;
}

int SoundConfig::getVolume() {
    return volume;
}