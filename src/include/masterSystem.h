/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

 #include "memory.h"
 #include "cpuZ80.h"
 #include "cartridge.h"
 #include "vdp.h"

class masterSystem {
public:
  masterSystem();
  ~masterSystem();
  bool start(std::string romFilename);
private:
  cpuZ80 *smsCPU;
  memory *smsMemory;
  cartridge *smsCartridge;
  vdp *smsVdp;
};
