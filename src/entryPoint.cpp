/*
Mastalgia - a (soon to be) Sega Master System emulator.
Lisenced under the GPLv3 lisence.
@author: Peter Savory
 */

#include <iostream>
#include "projectInfo.h"

int main()
{
  // Todo: Hide the long build string on master builds when the emulator is actually functional.
  std::cout<<PROJECT_NAME<<" "<<PROJECT_VERSION<<PROJECT_OS<<PROJECT_ARCH<< " (Compiled: " << __DATE__
    << " - " << __TIME__ << ") " << "branch: " << BRANCH_STRING << " - commit: " << CURRENT_COMMIT_STRING <<" starting..." << std::endl;
  return 0;
}
