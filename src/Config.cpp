//
// Created by Peter Savory on 23/06/2023.
//

#include <iostream>
#include "Config.h"
#include "Exceptions.h"

Config::Config() {
    // Set some default values
    displayWidth = 640;
    displayHeight = 480;
    fullScreenMode = false;
    preserveAspectRatio = false;

    player1Controls = new PlayerControlConfig();
    player1Controls->setDefaults();

    generalControls = new GeneralControlConfig();

#ifdef JSON_CONFIG_FILE

    std::string configFileName = "config.json"; // TODO allow this to be overridden with a command line parameter

    if (Utils::fileExists(configFileName)) {
        readConfigFile(configFileName);
    } else {
        // Populate defaults for optional fields that do not need to be configured
        generalControls->setDefaults();
        writeConfigFile(configFileName);
    }

#endif
}

int Config::getDisplayWidth() {

    if (displayWidth < 128) {
        // Prevent returning a number low enough to potentially cause problems
        throw ConfigurationException("Display width is too small");
    }

    return displayWidth;
}

int Config::getDisplayHeight() {

    if (displayHeight < 128) {
        // Prevent returning a number low enough to potentially cause problems
        throw ConfigurationException("Display height is too small");
    }

    return displayHeight;
}

bool Config::getPreserveAspectRatio() {
    return preserveAspectRatio;
}

bool Config::isFullScreenMode() {
    return fullScreenMode;
}

PlayerControlConfig *Config::getPlayer1ControlConfig() {
    return player1Controls;
}

PlayerControlConfig *Config::getPlayer2ControlConfig() {
    return nullptr; // TODO implement support for player 2
}

GeneralControlConfig *Config::getGeneralControlConfig() {
    return generalControls;
}

#ifdef JSON_CONFIG_FILE

void Config::readConfigFile(const std::string& fileName) {

    if (!Utils::fileExists(fileName)) {
        throw ConfigurationException(Utils::implodeString({"Config file '", fileName, "' does not exist"}));
    }

    json configFileJson = JsonHandler::parseJsonFile(fileName);

    if (JsonHandler::keyExists(configFileJson, "display")) {
        readDisplayConfigurationJson(configFileJson["display"]);
    }

    // TODO read "general" configuration when it is needed. (e.g display frame rate options, etc)

    if (JsonHandler::keyExists(configFileJson, "player1Controls")) {
        player1Controls->setFromConfig(configFileJson["player1Controls"]);
    } else {
        // No control scheme found - set defaults
        player1Controls->setDefaults();
    }

    if (JsonHandler::keyExists(configFileJson, "generalControls")) {
        generalControls->setFromConfig(configFileJson["generalControls"]);
    }

    // TODO read the player 2 control config. Don't bother populating it if it doesn't exist in the config file.
}

void Config::readDisplayConfigurationJson(json displayConfigurationJson) {

    if (JsonHandler::keyExists(displayConfigurationJson, "fullScreenMode")) {
        fullScreenMode = JsonHandler::getBoolean(displayConfigurationJson, "fullScreenMode");
    }

    if (JsonHandler::keyExists(displayConfigurationJson, "displayWidth")) {
        displayWidth = JsonHandler::getInteger(displayConfigurationJson, "displayWidth");
    }

    if (JsonHandler::keyExists(displayConfigurationJson, "displayHeight")) {
        displayHeight = JsonHandler::getInteger(displayConfigurationJson, "displayHeight");
    }

    if (JsonHandler::keyExists(displayConfigurationJson, "preserveAspectRatio")) {
        preserveAspectRatio = JsonHandler::getBoolean(displayConfigurationJson, "preserveAspectRatio");
    }

}

void Config::writeConfigFile(const std::string& fileName) {

    json output;

    json displayConfigurationJson = getDisplayConfigurationJson();

    output["display"] = displayConfigurationJson;

    json generalConfigurationJson = getGeneralConfigurationJson();

    if (!generalConfigurationJson.empty()) {
        output["general"] = generalConfigurationJson;
    }

    json player1ControllerConfigurationJson = player1Controls->getJson();

    if (!player1ControllerConfigurationJson.empty()) {
        // I don't think this should ever be empty, but handle it just in case.
        output["player1Controls"] = player1ControllerConfigurationJson;
    }

    // TODO save player 2 control config.

    json generalControlsConfigurationJson = generalControls->getJson();

    if (!generalControlsConfigurationJson.empty()) {
        output["generalControls"] = generalControlsConfigurationJson;
    }

    // Write the file, overwriting it if it already exists
    std::ofstream fileOut(fileName, std::ios::trunc);
    fileOut << output;
    fileOut.close();

    if (fileOut.bad()) {
        // Throw an error if writing the file has failed
        throw ConfigurationException(Utils::implodeString({"Unable to write to config file '", fileName, " '"}));
    }

}

json Config::getDisplayConfigurationJson() {
    json output;

    output["displayWidth"] = displayWidth;
    output["displayHeight"] = displayHeight;
    output["fullScreenMode"] = fullScreenMode;
    output["preserveAspectRatio"] = preserveAspectRatio;

    return output;
}

json Config::getGeneralConfigurationJson() {
    json output;
    // TODO nothing to store here right now - will be needed in future.
    return output;
}

#endif