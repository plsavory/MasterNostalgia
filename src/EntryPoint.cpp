/*
MasterNostalgia - a Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "Utils.h"
#include "Emulator.h"
#include "Exceptions.h"

int main(int argc, char *argv[]) {
    if (argc > 1 && std::string(argv[1]) == "-v") {
        std::cout << Utils::getVersionString(true) << std::endl;
        return 0;
    }

    // Start the Emulator
    try {
        Emulator *emulator = new Emulator();

        std::string romFileName;

        if (argc > 1) {
            romFileName = argv[1];
        }

        if (romFileName.empty()) {
            std::cout<<"No ROM file name has been provided."<<std::endl;
            return 0;
        }

        emulator->init(romFileName);
        emulator->run();
    } catch (GeneralException &e) {
        std::cout<<e.what()<<std::endl;
    } catch (std::exception &e) {
        std::cout<<"An exception has occurred but no error message was provided."<<std::endl;
    }

    return 0;
}
