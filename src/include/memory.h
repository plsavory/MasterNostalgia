/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

class memory {
public:
  memory();
  ~memory();
  unsigned char read(unsigned short location);
  void write(unsigned short location, unsigned char value);
};
