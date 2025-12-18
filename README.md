# nes-hanoi-tower

A homebrew NES game based on Tower of Hanoi

## Title screen

The title screen shows the title Tower of Hanoi, loops music to a chiptune version of "Jingle Bells",
and instructs the user to press start.

## Gameplay

When the game is started, initially the tower height is 1, and each subsequent level increases
the height, to a max of 8 for the eighth level and upon completion of that the game is won.

The colors of each block are starting from smallest to largest: magenta, red, orange, yellow, yellow-green, foreat green, teal, deep blue.

Selecting a block to move is done with the A button, then the left/right D-pad to move it between the three towers, then the A button again to place it.
Pressing the B button cancels picking up of the block, as does pressing A a second time on its original tower.

To pass a level, the player must use the minimum number of moves for that level's puzzle (i.e. 2^n - 1).

Each time a level is failed, the player loses a life, after 3 lives lost it's game over.

## Artistic style

Rudimentary 2D side-view of the tower, basically each tower loop can appear as a rectangle.

## Music

During gameplay, play a chiptune version of Ode to Joy (Beethoven's #9).

## Implementation

Implemented using C and the `cc65` compiler toolchain, appropriate makefiles to build a `.nes` file from source.

The `.nes` file should be playable on any decent NES emulator.

## Building

### Prerequisites

First, install the cc65 compiler toolchain:

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install cc65
```

**macOS (with Homebrew):**
```bash
brew install cc65
```

**Arch Linux:**
```bash
sudo pacman -S cc65
```

**Windows:**
1. Download the latest release from [cc65 GitHub releases](https://github.com/cc65/cc65/releases)
2. Extract the archive to a directory (e.g., `C:\cc65`)
3. Add the `bin` directory to your PATH environment variable

**From Source (any platform):**
```bash
git clone https://github.com/cc65/cc65.git
cd cc65
make
sudo make install
```

### Build the ROM

Once cc65 is installed:
```bash
make
```

This creates `build/hanoi.nes` which can be played on any NES emulator.

### Play the Game

```bash
# With FCEUX (recommended):
fceux build/hanoi.nes

# With Nestopia:
nestopia build/hanoi.nes

# Or use any other NES emulator
```

See [BUILD.md](BUILD.md) for additional build details and troubleshooting.

## Controls

- **D-Pad Left/Right**: Select tower
- **A Button**: Pick up / Place block
- **B Button**: Cancel (return block to original tower)
- **Start Button**: Begin game / Continue to next level
