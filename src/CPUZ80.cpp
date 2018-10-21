/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"

CPUZ80::CPUZ80(Memory *smsMemory)
{
  // Store a pointer to the memory object
  memory = smsMemory;

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
  stackPointer = 0xDFF0;

  for (int i = 0; i<9;i++) {
    gpRegisters[i].whole = 0x0;
  }

  state = cpuState::Running;
}

int CPUZ80::execute()
{
  // This function may be redundant - TODO: Call executeOpcode directly from MasterSystem class if this turns out to be the case in the future.
  if (state == cpuState::Running) {
    executeOpcode();
  }

  return 0;
}

void CPUZ80::executeOpcode()
{
  unsigned char opcode = NB();

  switch (opcode)
  {
    default:
    state = cpuState::Error;

    #ifdef VERBOSE_MODE
      std::cout<<"Error: Unknown opcode: 0x"<<std::hex<<(int)opcode<<std::endl;
    #endif

    break;
  }
}

/**
 * [CPUZ80::NB Fetch the next byte in memory]
 * @return [description]
 */
unsigned char CPUZ80::NB()
{
  return memory->read(++programCounter);
}
