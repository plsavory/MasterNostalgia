/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "Cartridge.h"
#include "Memory.h"
#include "CPUZ80.h"
#include "Utils.h"

/**
 * [cpuZ80::ldReg8 Load 8-bit value into destination]
 * @param dest  [Reference to destination location]
 * @param value [Value to be inserted into destination]
 */
void CPUZ80::ldReg8(unsigned char &dest, unsigned char value)
{
  dest = value;
}

/**
 * [cpuZ80::ldReg16 Load 16-bit value into destination register]
 * @param dest  [Reference to destination location]
 * @param value [Value to be inserted into destination]
 */
void CPUZ80::ldReg16(cpuRegister &dest, unsigned short value)
{
  dest.whole = value;
}

/**
 * [cpuZ80::adc ADC opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to add with what is stored in dest]
 */
void CPUZ80::adc(unsigned char &dest, unsigned char value)
{
// TODO: Implement this
}

/**
 * [cpuZ80::sbc SBC opcode handler]
 * @param dest  [Destination (Usually register A)]
 * @param value [Value to subtract with what is stored in dest]
 */
void CPUZ80::sbc(unsigned char &dest, unsigned char value)
{
// TODO: Implement this
}

void CPUZ80::setInterruptMode(int mode)
{
  interruptMode = mode;
  cyclesTaken = 8; // All im operations (as far as I know) take 8 cycles
}

/**
 * [CPUZ80::jpCondition Perform a conditional jump]
 * @param condition [The condition which needs to be true]
 * @param location  [Memory location to jump to]
 */
void CPUZ80::jpCondition(JPCondition condition, unsigned char location)
{

  bool conditionMet = false;

  switch (condition) {
    case JPCondition::NZ:
      conditionMet = !getFlag(CPUFlag::zero);
    break;
    case JPCondition::Z:
      conditionMet = getFlag(CPUFlag::zero);
    break;
    default:
    return;

    if (conditionMet) {
      // Get the next two bytes in memory and build an address to jump to
      unsigned short address = build16BitAddress();
      programCounter = address;
    }
  }
}
