/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "CPUZ80.h"

CPUZ80::CPUZ80()
{
  // Reset the CPU to its initial state
  reset();
}

CPUZ80::~CPUZ80()
{

}

void CPUZ80::reset()
{
  #ifdef VERBOSE_MODE
  std::cout<<"Resetting CPU..."<<std::endl;
  #endif

  programCounter = 0x0; // TODO: Give this the real value of the entry point of the Z80 binaries when I actually learn what that is.
  stackPointer = 0xFFFF;

  for (int i = 0; i<9;i++) {
    gpRegisters[i].whole = 0x0;
  }
}


int CPUZ80::execute()
{
  // TODO: Implement this
  return 0;
}
