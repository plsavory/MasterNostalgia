/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

class Memory {
public:
  Memory(Cartridge *cart);
  ~Memory();
  unsigned char read(unsigned short location);
  void write(unsigned short location, unsigned char value);
private:
  Cartridge *smsCartridge;
  unsigned char ram[0x4000];
  bool ramBanked; // Does cartridge include additional RAM?
};
