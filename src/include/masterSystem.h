/*
Mastalgia - a (soon to be) Sega Master System emulator.
Licensed under the GPLv3 license.
@author: Peter Savory
 */

 #include "Memory.h"
 #include "CPUZ80.h"
 #include "Cartridge.h"
 #include "VDP.h"

class MasterSystem {
public:
  MasterSystem();
  ~MasterSystem();
  bool start(std::string romFilename);
  void tick();
  void run();
private:
  CPUZ80 *smsCPU;
  Memory *smsMemory;
  Cartridge *smsCartridge;
  VDP *smsVdp;
};
