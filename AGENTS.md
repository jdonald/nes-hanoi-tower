# Repository Guidelines

## Project Structure

- `src/`: NES game source code (C and 6502 assembly).
  - C modules are organized by feature: `main.c` (state machine), `hanoi.c` (game rules), `input.c`, `music.c`, `text.c`, `sprite.c`.
  - Assembly files provide ROM/boot plumbing and assets: `header.s` (iNES header), `reset.s` (init/vectors), `chr_rom.s` (CHR tiles).
- `nes.cfg`: `ld65` linker/memory configuration (mapper 0 / NROM layout).
- `build/`: build outputs (ROM and intermediates). Treat as generated and avoid committing changes here.

## Build, Run, and Development

This project uses the `cc65` toolchain (`cc65`, `ca65`, `ld65`).

- `make`: builds the ROM at `build/hanoi.nes`.
- `make clean`: removes `build/` to ensure a clean rebuild.
- `make run`: prints example emulator commands.
- Emulator (example): `fceux build/hanoi.nes`

See `BUILD.md` for toolchain installation notes.

## Coding Style & Naming

- Indentation: 4 spaces; K&R-style braces.
- Naming: `snake_case` for functions/vars (`init_nes`, `frame_counter`), `*_t` for typedefs, `UPPER_SNAKE_CASE` for constants/macros.
- Prefer small, fixed-width-ish types common in cc65 code (`unsigned char`, `unsigned int`) and avoid heap allocation.
- Keep modules cohesive: add new features by extending an existing module or adding a new `src/<feature>.c` + `src/<feature>.h` pair.

## Testing Guidelines

No unit-test framework is currently set up; testing is primarily ROM smoke-testing in an emulator.

- Build: `make`
- Manual checks (minimum): title screen renders, `Start` begins gameplay, input moves/picks/places blocks, audio plays, win/lose screens appear.

## Commits & Pull Requests

- Commit messages follow short, imperative subjects (examples from history: “Add …”, “Fix …”, “Implement …”).
- PRs should include: what changed, how you tested (emulator + steps), and screenshots/GIFs for visual changes when practical.
- Avoid including generated artifacts (ROMs/objects) in commits; run `make clean` if you’re unsure.
