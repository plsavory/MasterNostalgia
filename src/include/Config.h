//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MASTALGIA_CONFIG_H
#define MASTALGIA_CONFIG_H

#ifdef JSON_CONFIG_FILE
#include "JsonHandler.hpp"
#endif

#include "PlayerControlConfig.h"

class Config {
public:

    Config();

    int getDisplayWidth();

    int getDisplayHeight();

    bool isFullScreenMode();

    bool getPreserveAspectRatio();

    PlayerControlConfig* getPlayer1ControlConfig();

    PlayerControlConfig* getPlayer2ControlConfig();

private:
    int displayWidth;

    int displayHeight;

    bool fullScreenMode;

    bool preserveAspectRatio;

    PlayerControlConfig *player1Controls;

#ifdef JSON_CONFIG_FILE

    void readConfigFile(const std::string& fileName);

    void readDisplayConfigurationJson(json displayConfigurationJson);

    json getDisplayConfigurationJson();

    json getGeneralConfigurationJson();

    void writeConfigFile(const std::string& fileName);

#endif

};

#endif //MASTALGIA_CONFIG_H
