# 99 boxes and expanded memory

This is an experimental build of Emerald Rogue EX v2.2.1, based on the upstream
`expansion` commit `a6adfcf18d7eaf99c2803e4b0bc04eca7af2f014`.
The review target is `Pokabbie/pokeemerald-rogue:expansion`.

It needs the matching [mGBA build](https://github.com/EESR-Bab/mgba/tree/enhanced-memory).
The emulator provides the extra memory, and the game changes let Rogue use it.
These limits apply inside the modified emulator; this will not run as intended
on a physical GBA.

| Resource | This build |
| --- | --- |
| PC storage | 99 boxes, 30 Pokemon each |
| Save chip | 1 MiB banked Flash |
| Main RAM | 1 MiB EWRAM |
| Internal RAM | 64 KiB IWRAM |
| ROM window | 64 MiB |
| Heap | 640 KiB |

The extra ROM space is mostly padding for now. The existing code and assets
still fit below 32 MiB. Tests check that the upper ROM window can be accessed.

## What changed

- The first ten boxes and Rogue's reserved data areas keep their existing
  layout. The other 89 boxes use the expanded Flash region, with two alternating
  saved copies and a temporary working area.
- Save generations, checksums and hashes tie the extra boxes to the correct
  base save. An original-format save imports with empty extra boxes. Existing
  expanded v1 saves keep their layout, and the original save backups are kept.
- The linker, initialization and allocator use the larger RAM ranges. The
  original IWRAM stacks and BIOS interrupt fields retain their addresses.
- Saving skips erase/program work for Flash sectors that have not changed.
  The modified emulator also uses shorter timings for the expanded chip.
  Ordinary Flash keeps its original timing.
- Closing PC storage skips decryption for completely zero-filled boxes and
  reads only the needed Flash segments. The tested B-button exit decreased
  from 205 to 137 frames on one copied save, about 3.43 to 2.29 seconds.
- This build also contains the earlier battle/AI changes and the option to
  release Eggs and Bad Eggs from the PC. Those have separate commits for review.
  Releasing an Egg still asks for confirmation.
- Small build-support changes avoid Windows command-line limits and libpng
  FILE/CRT mismatches. Tools are still built by default.

No art, audio or other game assets were changed for this prototype.

## Using it

Use the full Qt mGBA build from the paired fork. Markers in the ROM select the
memory profiles automatically, so there is no RAM setting to turn on.
Load the raw `.gba` build. The `.sav` belongs beside the ROM and uses the same
basename. An enhanced save is 1 MiB, optionally followed by a 16-byte RTC trailer.

Back up your save before importing it. When updating the ROM, use the normal
in-game Save and Continue, then make new emulator states. States contain runtime
pointers and need to match the game build; a new ROM can move those pointers.
Expanded saves require the modified emulator/game pair for their extra boxes.

## Building and review

The tested game configuration is `MODERN=1 RELEASE=1 EXPANSION=1`, using
ARM GNU GCC 13.2.1. Follow the repository's existing tool and dependency setup,
then select those options when invoking make. The game remains dependent on
the upstream asset-conversion tools and Poryscript. No compiler or generated
tool binaries are added by this branch.

The commits separate the earlier AI patch, 99-box storage,
Windows tool support, expanded RAM/ROM, PC-exit optimization and Egg release.
The final source set matches the compatible build's 33 changed or added files.
`enhanced-memory-source-manifest.json` records the normalized source hashes.

The recorded compatible game artifact is 67,108,864 bytes, SHA-256:

```text
d1aafdc920edf53bd58d07ad872b7a86236b628f8d9e147eb4e8247ebda02e4c
```

## Validation and limits

Checks so far cover RAM/ROM access, CPU/DMA/BIOS boundaries, large allocations,
saves and states, all 99 boxes, normal Save and Continue after a restart,
leaving the PC through its normal buttons, and 16 Egg-release and protection
cases. The emulator fork has more detail on its synthetic tests and Qt checks.
Tests that needed a save used private copies. Personal saves, states, settings
and screenshots are excluded from the public files.

The earlier AI patch passed focused C tests and ARM compilation. Some changes
only affect particular or defensive paths, and some AI predictions are still
approximations. The reported frozen battle has not been reproduced here.
There has not been a full adventure playthrough or a complete test of physical
controllers and peripherals. Tests used mGBA's HLE BIOS. Loading the enhanced
ELF directly as a game has not been validated.
