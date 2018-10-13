/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

/*
Master System memory map:
0x0000-0x3FFF : Rom Slot 1
0x4000-0x7FFF : Rom Slot 2
0x8000-0xBFFF : Rom Slot 3 / RAM Slot
0xC000-0xDFFF : RAM
0xE000-0xFFFF : Mirrored RAM
 */
#include "Memory.h"

Memory::Memory()
{

}

Memory::~Memory()
{

}

/**
 * [read Read from system memory]
 * @param  location [Memory location to read from]
 * @return          [Value within memory at location]
 */
unsigned char read(unsigned short location)
{
  // TODO: Implement this.
  return 0x0;
}

/**
 * [write Writes to system memory]
 * @param location [Memory location to write to]
 * @param value    [The byte to write]
 */
void write(unsigned short location, unsigned char value)
{
  if (location <= 0x8000) {
    // Attempting to write to ROM, disallow this...
    return;
  }

  /* Locations 0x8000-0xBFFF can also be ROM, depending on the cartridge.
   If writeable, this area of memory will be additional RAM included in the cartridge. */
  if (location <= 0xBFFF) {
    // Emulator doesn't support extended RAM right now, so temporarily just return here.
    // TODO: Fix this.
    return;
  }
}
