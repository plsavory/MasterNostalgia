/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <iterator>
#include "Cartridge.h"

Cartridge::Cartridge() {
    clearCartridge();
}

Cartridge::~Cartridge() {

}

/**
 * [cartridge::load Loads a cartridge into memory]
 * @param fileName [the path to the file]
 */
bool Cartridge::load(std::string fileName) {
#ifdef VERBOSE_MODE
    std::cout<<"Loading ROM: "<<fileName<<std::endl;
#endif

    // Determine the size of the ROM file
    struct stat fileStat{};
    int ROMSize;

    int fileStatus = stat(fileName.c_str(), &fileStat);

    // Do some standard checks on the file to decide on whether it is kosher or not (This is not 100%, but we at least should check obvious stuff)
    if (fileStatus != 0) {
        // File does not exist
        std::cout<<"Error: ROM file does not exist"<<std::endl;
        return false;
    }

    // Figure out the size of the ROM
    ROMSize = fileStat.st_size;

    if (ROMSize > MAX_CARTRIDGE_SIZE) {
        // ROM file too large - abort
        std::cout << "Error - ROM file is too large (" << ROMSize << ") bytes" << std::endl;
        return false;
    }

//    std::cout << ROMSize << " byte ROM found..." << std::endl;

    // Fill temp storage with data from ROM file
    typedef std::istream_iterator<unsigned char> istream_iterator;

    std::ifstream romStream(fileName, std::ios::binary); // file stream for reading file contents
    std::vector<unsigned char> tempStorage; // temporary storage for data - will be copied into cartridge struct later

    romStream >> std::noskipws; // Do not skip white space, we want every single character of the file.
    std::copy(istream_iterator(romStream), istream_iterator(),
              std::back_inserter(tempStorage)); // Copy the contents of the file into the temporary storage vector

    // Locate the ROM header (3 possible locations, might not be 100% needed as it is said to be always located at 7FF0, but check anyway just in case one of some of the cartridges wanted to be special)
    unsigned short romHeaderPossibleLocations[3] = {0x1FF0, 0x3FF0, 0x7FF0};

    unsigned char validRomText[8] = {0x54, 0x4D, 0x52, 0x20, 0x53, 0x45, 0x47, 0x41}; // TMR SEGA in hex

    bool headerFound;

    // Note: This 'header' isn't an actual file header, it is a piece of information included in the game ROMs for the USA/EU SMS/GG BIOS to varify the validity of the game.
    for (unsigned int i = 0; i <= 2; i++) {

        // Search for the usual ASCII text which should be present in all ROM headers to determine where it is.
        headerFound = false;
        int matchCount = 0;

        for (int i2 = 0; i2 < 8; i2++) {
            if (tempStorage[romHeaderPossibleLocations[i] + i2] == validRomText[i2]) {
                matchCount++;
            }
        }

        if (matchCount == 8) {
            // Valid ROM header found
            headerFound = true;

#ifdef VERBOSE_MODE
            std::cout<<"ROM Header found at: "<<std::hex<<"0x"<<romHeaderPossibleLocations[i]<<std::endl;
#endif

            break;
        }
    }

    /* Determine whether the cartridge is a Codemasters cartridge or not
    Codemasters ROMs include an extra header at 0x7FE0-0x7FE8 including some information, we only care about the checksum.

    Had trouble working out this bit myself, this bit is based on a solution from the following source:
    http://www.codeslinger.co.uk/pages/projects/mastersystem/starting.html

    This check is required as these games use their own separate memory mapper which does not work like the standard one.
    */

    unsigned short checksum = tempStorage[0x7FE7] << 8;
    checksum |= tempStorage[0x7FE6];

    if (checksum == 0x0) {
        isCodemastersCart = false;
    } else {

        unsigned short checksumTest = 0x10000 - checksum;

        unsigned short answer = tempStorage[0x7FE9] << 8;

        answer |= tempStorage[0x7FE8];

        isCodemastersCart = (checksumTest == answer);
    }

    // Determine region of the cartridge (TODO: Make this work better in future, relying on the TMR SEGA text being present is not really a reliable way)
    if (headerFound) {
        region = CartridgeRegion::USA; // Assume USA for now, TODO: add some detection for PAL regions
    } else {
        region = CartridgeRegion::Japan;
    }

    // Figure out if the cartridge is larger than 1mb, needed later for memory paging
    megCartridge = ROMSize > 0x80000;

    // Copy the ROM contents into system memory, excluding the actual ROM header at the start of the file

    // Strip the ROM's header if required
    if ((ROMSize % 0x4000) == 512) {

        for (int i = 512; i <= ROMSize - 512; i++) {
            cartridgeData[i - 512] = tempStorage[i];
        }

    } else {

        for (int i = 0; i < ROMSize; i++) {
            cartridgeData[i] = tempStorage[i];
        }

    }

    return true;
}

/**
 * [cartridge::clearCartridge Clear cartridge data so that nothing remains upon reload]
 */
void Cartridge::clearCartridge() {
    for (unsigned char & i : cartridgeData) {
        i = 0x0;
    }

#ifdef VERBOSE_MODE
    std::cout<<"Clearing cartridge data..."<<std::endl;
#endif
}

/**
 * [Cartridge::isCodemasters Return if the cartridge is a Codemasters cartridge]
 * @return [True if cartridge is a Codemasters one]
 */
bool Cartridge::isCodemasters() {
    return isCodemastersCart;
}

/**
 * [Cartridge::isMegCartridge Is the cartridge a megabit cartridge or not]
 * @return [description]
 */
bool Cartridge::isMegCartridge() {
    return megCartridge;
}

unsigned char Cartridge::read(unsigned long location) {
    return cartridgeData[location];
}
