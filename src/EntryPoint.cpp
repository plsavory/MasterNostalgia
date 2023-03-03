/*
Mastalgia - a (soon to be) Sega Master System emulator.
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

        std::string romFile;

        if (argc == 1) {
            romFile = "roms/zexdoc.sms";
        } else {
            romFile = argv[1];
        }

        emulator->init(romFile);
        emulator->run(); // Main loop
    } catch (GeneralException &e) {
        std::cout<<e.what()<<std::endl;
    } catch (std::exception &e) {
        std::cout<<"An exception has occurred but no error message was provided."<<std::endl;
    }

    return 0;
}
