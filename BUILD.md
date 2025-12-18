# Building the NES Tower of Hanoi Game

## Prerequisites

You need the cc65 compiler toolchain to build this NES ROM.

### Installing cc65

**On Ubuntu/Debian:**
```bash
sudo apt-get install cc65
```

**On macOS (with Homebrew):**
```bash
brew install cc65
```

**On Windows:**
Download and install from: https://github.com/cc65/cc65/releases

## Building the ROM

Once cc65 is installed, simply run:

```bash
make
```

This will create `build/hanoi.nes` which can be played on any NES emulator.

## Running the Game

You can use any NES emulator to play the game:

**FCEUX (recommended):**
```bash
fceux build/hanoi.nes
```

**Nestopia:**
```bash
nestopia build/hanoi.nes
```

**Other emulators:**
- Mesen
- RetroArch with NES core
- Any web-based NES emulator

## Clean Build

To clean the build directory:

```bash
make clean
```

## Project Structure

```
nes-hanoi-tower/
├── src/              # Source code
│   ├── main.c        # Main game loop
│   ├── hanoi.c       # Tower of Hanoi logic
│   ├── music.c       # Music/audio system
│   ├── input.c       # Controller input
│   ├── text.c        # Text rendering
│   ├── header.s      # iNES header
│   ├── reset.s       # NES initialization
│   └── chr_rom.s     # Graphics data
├── build/            # Build output
├── Makefile          # Build configuration
└── nes.cfg           # Linker configuration
```

## Troubleshooting

**Build fails with missing headers:**
- Ensure cc65 is properly installed
- Check that cc65 libraries are in your PATH

**ROM doesn't run in emulator:**
- Verify the emulator supports mapper 0 (NROM)
- Try a different emulator
- Check that the build completed without errors

**No sound:**
- Some emulators have audio disabled by default
- Check emulator audio settings
