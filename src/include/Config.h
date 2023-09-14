//
// Created by Peter Savory on 23/06/2023.
//

#ifndef MasterNostalgia_CONFIG_H
#define MasterNostalgia_CONFIG_H

#ifdef JSON_CONFIG_FILE
#include "JsonHandler.hpp"
#endif

#include "PlayerControlConfig.h"
#include "GeneralControlConfig.h"
#include "SoundConfig.h"

class Config {
public:

    Config();

    int getDisplayWidth();

    int getDisplayHeight();

    bool isFullScreenMode();

    bool getPreserveAspectRatio();

    bool getPauseEmulationWhenNotInFocus();

    bool getHideMouseCursor();

    bool getPALOutputMode();

    bool getPersistCRAM();

    std::string getCRAMSaveLocation();

    bool getAutoSaveCRAM();

    PlayerControlConfig* getPlayer1ControlConfig();

    PlayerControlConfig* getPlayer2ControlConfig();

    GeneralControlConfig* getGeneralControlConfig();

    SoundConfig* getSoundConfig();

private:
    int displayWidth;

    int displayHeight;

    bool fullScreenMode;

    bool preserveAspectRatio;

    bool pauseEmulationWhenNotInFocus;

    bool hideMouseCursor;

    bool PALOutputMode;

    bool persistCRAM;

    bool autoSaveCRAM;

    std::string CRAMSaveLocation;

    PlayerControlConfig *player1Controls;

    GeneralControlConfig *generalControls;

    SoundConfig *soundConfig;

#ifdef JSON_CONFIG_FILE

    void readConfigFile(const std::string& fileName);

    void readDisplayConfigurationJson(json displayConfigurationJson);

    json getDisplayConfigurationJson();

    json getGeneralConfigurationJson();

    json getSystemConfigurationJson();

    void writeConfigFile(const std::string& fileName);

    void readGeneralConfigurationJson(json generalConfigurationJson);

    void readSystemConfigurationJson(json systemConfigurationJson);

#endif

};

#endif //MasterNostalgia_CONFIG_H
