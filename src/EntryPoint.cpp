/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 license.
@author: Peter Savory
 */

#include <iostream>
#include "ProjectInfo.h"
#include "MasterSystem.h"

int main(int argc, char* argv[])
{
  // Todo: Hide the long build string on master builds when the emulator is actually functional.
  std::cout<<PROJECT_NAME<<" "<<PROJECT_VERSION<<PROJECT_OS<<PROJECT_ARCH<< " (Compiled: " << __DATE__
    << " - " << __TIME__ << ") " << "branch: " << BRANCH_STRING << " - commit: " << CURRENT_COMMIT_STRING <<" starting..." << std::endl;

  // Start the Emulator
  MasterSystem *emulator = new MasterSystem();

  std::string romFile;

  if (argc == 1) {
    romFile = "roms/test.sms";
  } else {
    romFile = argv[1];
  }

  emulator->start(romFile);

  return 0;
}
