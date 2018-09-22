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

  cpuZ80* testCPU = new cpuZ80();

  // Do some tests on the registers
  testCPU->ldReg8(testCPU->gpRegisters[cpuReg::AF].hi,0xFF);
  testCPU->ldReg8(testCPU->gpRegisters[cpuReg::AF].lo,0xAC);
  std::cout<<"A = 0x"<<std::hex<<(int)testCPU->gpRegisters[cpuReg::AF].hi<<std::endl;
  std::cout<<"AF = 0x"<<std::hex<<(int)testCPU->gpRegisters[cpuReg::AF].whole<<std::endl;

  testCPU->ldReg8(testCPU->gpRegisters[cpuReg::BC].hi,0xAA);
  testCPU->ldReg8(testCPU->gpRegisters[cpuReg::BC].lo,0x12);
  std::cout<<"B = 0x"<<std::hex<<(int)testCPU->gpRegisters[cpuReg::BC].hi<<std::endl;
  std::cout<<"BC = 0x"<<std::hex<<(int)testCPU->gpRegisters[cpuReg::BC].whole<<std::endl;

  testCPU->ldReg16(testCPU->gpRegisters[cpuReg::BC],0xEEDD);
  std::cout<<"B = 0x"<<std::hex<<(int)testCPU->gpRegisters[cpuReg::BC].hi<<std::endl;
  std::cout<<"BC = 0x"<<std::hex<<(int)testCPU->gpRegisters[cpuReg::BC].whole<<std::endl;


}

cpuTester::~cpuTester()
{

}
