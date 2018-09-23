/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "masterSystem.h"

masterSystem::masterSystem()
{
  // Initialize the Emulator

  #ifdef VERBOSE_MODE
  std::cout<<"Initializing Emulator..."<<std::endl;
  #endif

  smsCartridge = new cartridge();
  smsMemory = new memory();
  smsCPU = new cpuZ80();
  smsVdp = new vdp();
}

masterSystem::~masterSystem()
{

}

bool masterSystem::start(std::string romFilename)
{
  // TODO: Implement this.
  return false;
}
