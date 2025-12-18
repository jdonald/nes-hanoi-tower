# NES Tower of Hanoi - Implementation Summary

## Overview

This is a complete implementation of the Tower of Hanoi puzzle game for the Nintendo Entertainment System (NES), written in C using the cc65 compiler toolchain.

## Features Implemented

### Title Screen
- Displays "TOWER OF HANOI" title
- Shows "PRESS START" instruction
- Plays Jingle Bells music in a loop

### Gameplay
- Progressive difficulty: 8 levels with 1-8 blocks
- Color-coded blocks (smallest to largest):
  - Magenta → Red → Orange → Yellow → Yellow-Green → Forest Green → Teal → Deep Blue
- Minimum move requirement (2^n - 1 moves)
- 3-life system
- Move counter

### Controls
- **D-Pad Left/Right**: Move cursor between towers
- **A Button**: Pick up block / Place block
- **B Button**: Cancel and return block to original tower
- **Start Button**: Begin game / Progress to next level

### Music System
- **Title Screen**: Jingle Bells (chiptune)
- **Gameplay**: Ode to Joy from Beethoven's 9th Symphony (chiptune)
- Looping music with proper frame-based timing

### Game States
1. **Title Screen**: Waits for start button
2. **Gameplay**: Main game logic with Tower of Hanoi rules
3. **Level Complete**: Shows success message when level passed optimally
4. **Level Failed**: Deducts a life when too many moves used
5. **Game Over**: When all lives are lost
6. **Victory**: After completing all 8 levels

## Technical Architecture

### File Structure

**C Source Files:**
- `main.c` - Main game loop and state machine
- `hanoi.c` - Tower of Hanoi game logic
- `music.c` - APU music playback system
- `input.c` - Controller input handling
- `text.c` - Text rendering utilities

**Header Files:**
- `nes.h` - NES hardware register definitions
- `hanoi.h` - Game logic interface
- `music.h` - Music system interface
- `input.h` - Input handling interface
- `text.h` - Text rendering interface

**Assembly Files:**
- `header.s` - iNES ROM header
- `reset.s` - NES initialization and reset vectors
- `chr_rom.s` - Character ROM data (graphics tiles)

**Build Files:**
- `Makefile` - Build configuration
- `nes.cfg` - Linker memory configuration

### Memory Layout

- **PRG-ROM**: 2 × 16KB banks for program code
- **CHR-ROM**: 1 × 8KB bank for graphics
- **Mapper**: 0 (NROM) - simplest and most compatible
- **Mirroring**: Vertical

### Graphics System

- **Tiles**: Custom 8×8 pixel tiles including:
  - Block tiles (solid rectangles)
  - Tower pole and base
  - Numbers 0-9
  - Alphabet A-Z
  - Special characters

- **Palettes**: 4 background palettes with game colors

### Audio System

- Uses NES APU Pulse Channel 1 for melody
- Frame-based note timing system
- Two complete song implementations:
  - Jingle Bells (title screen)
  - Ode to Joy (gameplay)
- Automatic looping

### Game Logic

**Tower of Hanoi Rules:**
- Only one disk can be moved at a time
- Only the top disk on a stack can be moved
- A larger disk cannot be placed on a smaller disk
- Goal: Move all disks from leftmost to rightmost tower

**Winning Conditions:**
- All blocks must be on the rightmost tower
- Must be achieved in minimum moves (2^n - 1)
- Exceeding minimum moves = life lost

**Progression:**
- Level 1: 1 block (1 move required)
- Level 2: 2 blocks (3 moves required)
- Level 3: 3 blocks (7 moves required)
- ...up to...
- Level 8: 8 blocks (255 moves required)

## Building and Testing

The project requires the cc65 toolchain. See [BUILD.md](BUILD.md) for instructions.

```bash
make                    # Build the ROM
fceux build/hanoi.nes   # Test in emulator
```

## Compatibility

The ROM uses mapper 0 (NROM), making it compatible with:
- All NES emulators
- Flash cartridges
- Original NES hardware (with appropriate cartridge)

Tested configurations:
- Uses standard NES resolution (256×240)
- NTSC timing
- No special hardware requirements

## Code Quality

- Well-organized modular architecture
- Separation of concerns (input, graphics, audio, game logic)
- Documented header files
- Const data for music scores
- Proper NES initialization sequence
- Efficient frame-based updates

## Future Enhancements (Not Implemented)

Potential improvements for future versions:
- High score saving (battery-backed RAM)
- Sound effects for moves
- Enhanced graphics with sprites
- Animation for block movements
- Timer display
- 2-player mode
