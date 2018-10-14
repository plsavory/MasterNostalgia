/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
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

Cartridge::Cartridge()
{
  clearCartridge();
}

Cartridge::~Cartridge()
{

}

/**
 * [cartridge::load Loads a cartridge into memory]
 * @param fileName [the path to the file]
 */
bool Cartridge::load(std::string fileName)
{
  #ifdef VERBOSE_MODE
    std::cout<<"Loading ROM: "<<fileName<<std::endl;
  #endif

  // Determine the size of the ROM file
  struct stat fileStat;

  int fileStatus = stat(fileName.c_str(),&fileStat);

  // Do some standard checks on the file to decide on whether it is kosher or not (This is not 100%, but we at least should check obvious stuff)
  if (fileStatus == 0) {

    // Figure out the size of the ROM
    int ROMSize = fileStat.st_size;

    if (ROMSize > MAX_CARTRIDGE_SIZE)
    {
      // ROM file too large - abort
      std::cout << "Error - ROM file is too large (" << ROMSize << ") bytes" << std::endl;
      return false;
    }
    else
    {
      std::cout << ROMSize << " byte ROM found..." << std::endl;
    }

  } else {

    // File does not exist
    #ifdef VERBOSE_MODE
      std::cout<<"Error: ROM file does not exist"<<std::endl;
    #endif

    return false;

  }

  // Fill temp storage with data from ROM file
  typedef std::istream_iterator<unsigned char> istream_iterator;

  std::ifstream romStream(fileName,std::ios::binary); // file stream for reading file contents
	std::vector<unsigned char> tempStorage; // temporary storage for data - will be copied into cartridge struct later

  romStream >> std::noskipws; // Do not skip white space, we want every single character of the file.
	std::copy(istream_iterator(romStream),istream_iterator(),std::back_inserter(tempStorage)); // Copy the contents of the file into the temporary storage vector

  // Locate the ROM header (3 possible locations, might not be 100% needed as it is said to be always located at 7FF0, but check anyway just in case one of some of the cartridges wanted to be special)
  unsigned short romHeaderPossibleLocations[3] = {0x1FF0,0x3FF0,0x7FF0};

  unsigned short validRomText[8] = {0x54,0x4D,0x52,0x20,0x53,0x45,0x47,0x41}; // TMR SEGA in hex

  unsigned short headerLocation = 0x0;

  for (unsigned int i = 0; i<=2;i++) {

    // Search for the usual ASCII text which should be present in all ROM headers to determine where it is.
    bool headerFound = false;
    int matchCount = 0;

    for (int i2 = 0; i2<8; i2++) {
      if (tempStorage[romHeaderPossibleLocations[i]+i2] == validRomText[i2]) {
        matchCount++;
      }
    }

    if (matchCount == 8) {
      // Valid ROM header found
      headerLocation = romHeaderPossibleLocations[i];
      headerFound = true;

      #ifdef VERBOSE_MODE
        std::cout<<"ROM Header found at: "<<std::hex<<"0x"<<romHeaderPossibleLocations[i]<<std::endl;
      #endif

      break;
    }
  }

  // Process the ROM header - this will also check to see if the ROM is valid
  unsigned char romHeader[0xF];
  int headerMessage = processHeader(romHeader);

  if (!headerMessage) {

    #ifdef VERBOSE_MODE
      std::cout<<"Error: ROM is not a valid Master System ROM"<<std::endl;
    #endif

  }

  return false;
}

/**
 * [cartridge::clearCartridge Clear cartridge data so that nothing remains upon reload]
 */
void Cartridge::clearCartridge()
{
  for (int i = 0; i<MAX_CARTRIDGE_SIZE;i++) {
    cartridgeData[i] = 0x0;
  }

  #ifdef VERBOSE_MODE
    std::cout<<"Clearing cartridge data..."<<std::endl;
  #endif
}

/**
 * [Cartridge::processHeader Process the ROM header - Returns false upon invalid SMS ROM header]
 * @return [description]
 */
int Cartridge::processHeader(unsigned char romHeader[0xF])
{
  // TODO: Implement this
  return 0;
}
