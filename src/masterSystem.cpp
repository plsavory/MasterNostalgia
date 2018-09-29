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
  // TODO: Load a ROM into memory

  // Enter the main loop
  run();

  return true;
}

void masterSystem::tick()
{

int z80ClockCycles = smsCPU->execute();

int machineClicks = z80ClockCycles*3;

float vdpCycles = machineClicks/2;

int soundCycles = z80ClockCycles;

smsVdp->execute(vdpCycles);

// TODO: Update sound when it exists

  return;
}

void masterSystem::run()
{
  // TODO: Implement this
}
