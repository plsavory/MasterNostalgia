# MasterNostalgia

A Sega Master System emulator written in C++, developed as an educational exercise in understanding how the hardware works at a low level.

## Features

- Zilog Z80 CPU emulation — passes all [zexall](https://mdfs.net/Software/Z80/Exerciser/) tests
- VDP emulation with NTSC and PAL display modes (192, 224, and 240 line modes)
- Partial PSG (SN76489) sound emulation — tone channels implemented, noise channel in progress
- Sega and Codemasters cartridge mapper support
- Partial SG-1000 compatibility (VDP Mode 2)
- Fullscreen mode (F11)
- Configurable controls and display settings via `config.json`
- Aspect ratio preservation (letterbox) or stretched display
- Pause emulation when window loses focus (configurable)

## Usage

Pass a path to a ROM file as a command line argument:

```
./MasterNostalgia "path/to/rom.sms"
```

You can also drag a ROM file onto the executable in your file manager.

## Controls

| Action | Default Key |
|---|---|
| D-pad | Arrow keys |
| Button 1 | A |
| Button 2 | S |
| Pause | P |
| Fullscreen | F11 |
| Quit | Escape |

Controls can be remapped in `config.json`, which is created automatically on first launch.

## Building

### Requirements

- [CMake](https://cmake.org/) 3.15 or newer
- [SDL3](https://www.libsdl.org/)
- A C++14-compatible compiler

SDL3 can be installed via your system package manager, or built from source. On macOS, Homebrew can be used:

```
brew install sdl3
```

### Compiling

```bash
git clone <repository url>
cd MasterNostalgia
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
```

## Known Limitations

- Sound emulation is experimental — the noise channel is not yet implemented
- Timing is approximate and known to deviate slightly from real hardware
- No gamepad support yet (keyboard only)
- Game Gear support is not yet implemented
- FM sound unit (YM2413) is not emulated

## References

The following resources were used during development:

**Z80**
- [clrhome Z80 instruction table](https://clrhome.org/table/)
- [Z80 Heaven instruction set reference](http://z80-heaven.wikidot.com/instructions-set)
- [The Undocumented Z80 Documented (PDF)](http://www.z80.info/zip/z80-documented.pdf)
- [drhelius Z80 reference](https://gist.github.com/drhelius/8497817)

**Master System hardware**
- [Code Slinger — Master System](http://www.codeslinger.co.uk/pages/projects/mastersystem/hardware.html)
- [SMS Power development wiki](https://www.smspower.org/Development/Index)
- [Sega Master System Technical Documentation (1998)](https://segaretro.org/Sega_Master_System_Technical_Documentation_(1998-06-10))
- [SMS VDP documentation](https://github.com/franckverrot/EmulationResources/blob/master/consoles/sms-gg/Sega%20Master%20System%20VDP%20documentation.txt)
- [TMS9918 datasheet (PDF)](http://www.cs.columbia.edu/~sedwards/papers/TMS9918.pdf)

## License

This project is licensed under the GNU General Public License v3.0 — see [license.txt](license.txt) for details.
