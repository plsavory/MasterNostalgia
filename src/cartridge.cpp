/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "cartridge.h"

cartridge::cartridge()
{
  clearCartridge();
}

cartridge::~cartridge()
{

}

/**
 * [cartridge::load Loads a cartridge into memory]
 * @param fileName [the path to the file]
 */
bool cartridge::load(std::string fileName)
{
  // TODO: Implement this
  return false;
}

/**
 * [cartridge::clearCartridge Clear cartridge data so that nothing remains upon reload]
 */
void cartridge::clearCartridge()
{
  for (int i = 0; i<MAX_CARTRIDGE_SIZE;i++) {
    cartridgeData[i] = 0x0;
  }

  #ifdef VERBOSE_MODE
    std::cout<<"Clearing cartridge data..."<<std::endl;
  #endif
}
