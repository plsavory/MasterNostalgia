#ifndef MasterNostalgia_SOUNDCONFIG_H
#define MasterNostalgia_SOUNDCONFIG_H

#include "JsonHandler.hpp"

class SoundConfig {
public:
    SoundConfig();

#ifdef JSON_CONFIG_FILE
    json getSoundConfigurationJson();

    void setFromConfig(json soundConfigurationJson);

#endif

    bool isEnabled();

    int getVolume();

private:
    bool enabled;

    int volume;
};

#endif //MasterNostalgia_SOUNDCONFIG_H
