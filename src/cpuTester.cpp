/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "cpuTester.h"
#include "cpuZ80.h"

/**
 * [cpuTester::cpuTester Run some CPU tests]
 *
 * Currently no ROM-loading code exists within the emulator. For now, test CPU functions here until that is implemented.
 */
cpuTester::cpuTester()
{
  std::cout<<"Testing the CPU..."<<std::endl;

  cpuZ80* testCPU = new cpuZ80;
}

cpuTester::~cpuTester()
{

}
