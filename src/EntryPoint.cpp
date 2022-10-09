/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "Utils.h"
#include "MasterSystem.h"
#include "Exceptions.h"

int main(int argc, char *argv[]) {
    // Todo: Hide the long build string on master builds when the emulator is actually functional.
    std::cout << Utils::getVersionString(true) << " starting..." << std::endl;

    // Start the Emulator
    try {
        MasterSystem *emulator = new MasterSystem();

        std::string romFile;

        if (argc == 1) {
            romFile = "roms/test.sms";
        } else {
            romFile = argv[1];
        }

        emulator->start(romFile);
    } catch (GeneralException &e) {
        std::cout<<e.what()<<std::endl;
    } catch (std::exception &e) {
        std::cout<<"An exception has occurred but no error message was provided."<<std::endl;
    }

    return 0;
}
