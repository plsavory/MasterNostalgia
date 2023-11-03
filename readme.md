# MasterNostalgia

MasterNostalgia is a Sega Master System and SG-1000 emulator written in C++ (using SFML for I/O).

This emulator is being developed as an educational exercise to learn how to emulate these systems as well as improving my own programming and logic skills.

MasterNostalgia has been tested on and runs on the following operating systems:

- Windows
- macOS
- GNU/Linux distributions

## Usage

To run a ROM: provide a path to a ROM file name as the first command line parameter when launching on the command line
(or drag a ROM file onto the emulator's executable using your operating system's file manager).

Example:

~~~console
./MasterNostalgia "roms/zexall.sms"
~~~

To output information about the version/build of the emulator and exit, use the "-v" parameter.

Example:
~~~
./MasterNostalgia -v
~~~

## Default Controls

- Control pad 1 button 1: A
- Control pad 1 button 2: S
- Control pad 1 d-pad: Arrow keys
- Pause: P
- Quit: ESC
- SHIFT + (Number 0-9): Save game state
- CTRL + (Number 0-9): Load game state

## Configuring the emulator

When first running the emulator, a config.json file will be created in the same directory as the emulator's executable. Many aspects of the emulator can be configured by editing this file.

The auto-created JSON file will include all default settings for the emulator.

## Common issues/troubleshooting

### Save states not saving/loading:

The directory to store save states in is defined in the config.json file (Key: general/saveStateLocation) ("sstates" in the same directory as the emulator's executable by default).

Ensure that this directory exists and that the user you are running the emulator as has read/write access to this directory.

Note: If there is no directory defined for save states in the config file, the emulator will attempt to store them in the same directory as the currently-running ROM file.

### Game save progress/CRAM not persisting:

The directory to store game save data/CRAM in is defined in the config.json file (Key: general/CRAMSaveLocation) ("cram" in the same directory as the emulator's executable by default).

Ensure that this directory exists and that the user you are running the emulator as has read/write access to this directory.

Ensure that the option to store CRAM/game save data is turned on in the config.json file (Key: general/persistCRAM), it is enabled by default.

Turn on the general/autoSaveCRAM option in the config.json file to make the emulator monitor the emulated CRAM and automatically save it after it hasn't been written to for a short time.

Note: If there is no directory defined for game saves/CRAM storage in the config file, the emulator will attempt to store them in the same directory as the currently-running ROM file (If the persistCRAM option is turned on).

Note 2: If anything is present in CRAM and the general/persistCRAM option is turned on in the config file, the emulator will attempt to save it to a file upon exiting.

## Compiling/Building

Requires the SFML 2.5.1 library to be installed on your system compiled for your C++ compiler (https://www.sfml-dev.org/), SFML
has pre-built binaries for some compiler versions, you can also choose to compile it yourself or install it through your
Linux distribution's package manager (or Homebrew on macOS). Please see their website for more details.

Also requires CMake (https://cmake.org/), this can usually be installed through your operating system's package manager.

Once your system has the above packages installed, the following terminal commands should build the project:

~~~console
cd (directory where MasterNostalgia was cloned into)

mkdir build

cd build

cmake ..

make
~~~
(Replace "make" with mingw32-make if using MinGW to build on Windows.)

## Resources used during development

Throughout developing this project I am using the following information sources

### Z80 Instruction Set & flag behaviour

- https://clrhome.org/table/
- http://z80-heaven.wikidot.com/instructions-set
- http://www.z80.info/zip/z80-documented.pdf
- https://gist.github.com/drhelius/8497817

### Emulator development information and code examples

The following resource has been used heavily while developing this project, some parts of the project (notably the VDP and PSG implementations) are heavily based on this.
- http://www.codeslinger.co.uk/pages/projects/mastersystem.html

### General Master System hardware information
- https://www.smspower.org/Development/Index
- https://segaretro.org/Sega_Master_System_Technical_Documentation_(1998-06-10)
- https://github.com/franckverrot/EmulationResources/blob/master/consoles/sms-gg/Sega%20Master%20System%20VDP%20documentation.txt
- http://www.cs.columbia.edu/~sedwards/papers/TMS9918.pdf