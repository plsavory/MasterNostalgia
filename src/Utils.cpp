/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#include "Utils.h"

/**
 * [Utils::setBit Sets or unsets a bit on the provided 8-bit variable]
 * @param bit      [Which bit to operate on]
 * @param val      [New value]
 * @param variable [The variable to set/unset the bit on]
 */
void Utils::setBit(int bit, bool val, unsigned char &variable)
{
  // Can't set a bit greater than the size of the variable
  if (bit > 7) {
    return;
  }

  if (val) {
    variable |= (1 << bit);
  } else {
    variable &= ~(1 << bit);
  }
}

/**
 * [Utils::setBit Sets or unsets a bit on the provided 16-bit variable]
 * @param bit      [Which bit to operate on]
 * @param val      [New value]
 * @param variable [The variable to set/unset the bit on]
 */
void Utils::setBit(int bit, bool val, unsigned short &variable)
{
  // Can't set a bit greater than the size of the variable
  if (bit > 15) {
    return;
  }

  if (val) {
    variable |= (1 << bit);
  } else {
    variable &= ~(1 << bit);
  }
}

/**
 * [Utils::testBit Tests a bit]
 * @param  bit      [The bit to test]
 * @param  variable [The data to do the test on]
 * @return          [True if bit is set, otherwise false]
 */
bool Utils::testBit(int bit, unsigned char variable)
{
  if (bit > 7)
    return false;

  return (variable & (1 << bit));
}

/**
 * [Utils::testBit Tests a bit]
 * @param  bit      [The bit to test]
 * @param  variable [The data to do the test on]
 * @return          [True if bit is set, otherwise false]
 */
bool Utils::testBit(int bit, unsigned short variable)
{
  if (bit > 15)
    return false;

  return (variable & (1 << bit));
}
