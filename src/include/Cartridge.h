/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

#define MAX_CARTRIDGE_SIZE 0x100000

enum CartridgeRegion {
  Japan, USA, Europe, Unknown
};

class Cartridge {
public:
  Cartridge();
  ~Cartridge();
  bool load(std::string fileName);
private:
  int currentBankState;
  bool isCodemasters;
  unsigned char cartridgeData[MAX_CARTRIDGE_SIZE];
  void clearCartridge();
  CartridgeRegion region;
  bool megCartridge;
};
