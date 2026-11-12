# Commands

Command line flags for the game binary. Pass any number of them, in any order:

```bash
./bin/zoxel_linux_arm_opengl_sdl_dev/zoxel-dev.bin --autostart --tiny --windowed
```

The binary lives in `bin/zoxel_<os>_<arch>_..._dev/` (debug) or the matching release folder,
see [Onboarding](onboarding.md#2-build-and-run). Run it from its own folder, which is what
`make rund` and `bsh/tst.sh` do.

Each module parses its own flags, with a `process_arguments_*` function registered by
`add_hook_terminal_command`. To add one, find the module's `fun/terminal.c` (or `arguments.c`)
and add it to this doc and to `--help` too.

`-h` / `--help` prints a short version of this list and exits. Its text is in
[help.c](../src/_/terminals/fun/help.c), keep it in sync when adding a flag.

## Window and Display

| Flag | Effect | Source |
|---|---|---|
| `-w` `--windowed` | windowed mode | [apps](../src/apps/fun/arguments.c) |
| `-f` `--fullscreen` | fullscreen, overrides the saved setting | apps |
| `-g` `--halfscreen` | half screen sized window | apps |
| `-s` `--splitscreen` | split screen local coop | apps |
| `--nodecor` | no window decorations | apps |
| `--background` | hidden window, no sound, no mouse lock or focus pause (tests use it) | apps |
| `-z` `--headless` | no window | apps |
| `--vsync` `--enablevsync` | vsync on | apps |
| `--novsync` `--disablevsync` | vsync off | apps |
| `-tu` `--tinyui` | UI scale 1 | [ui](../src/ui/fun/arguments.c) |
| `-su` `--smallui` | UI scale 2 | ui |

## Graphics

| Flag | Effect | Source |
|---|---|---|
| `--gles` | use OpenGL ES straight away, skipping the desktop GL check | apps |
| `--es` `--opengles` | use OpenGL ES | [sdl](../src/apps/sdl/set/terminal.c) |
| `--openglcore` | use desktop OpenGL core | sdl |
| `-e` | OpenGL ES override for the renderer | [rendering](../src/rendering/fun/terminal.c) |
| `-v` `--vulkan` | Vulkan backend (experimental) | rendering |
| `--verbose` | verbose logs, i.e. prints `GL_VERSION` / `GL_RENDERER` | sdl |

On boards without desktop GL (i.e. the Mali boards), the game falls back to ES by itself.

## Game

| Flag | Effect | Source |
|---|---|---|
| `--autostart` | skips the menus, starts a new game | [huds](../src/huds/fun/terminal.c) |
| `--tiny` | terrain stream distance 3, height 3 | [terrain](../src/terrain/fun/terminal.c) |
| `--medium` | terrain stream distance 6, height 3 | terrain |
| `--large` | terrain stream distance 12, height 4 | terrain |
| `--freeroam` | free camera, not attached to the player character | [games](../src/games/dat/rules.c) |
| `--nonpc` `--nonpcs` | no npcs spawn | [characters](../src/characters/set/_.c) |
| `--noblockvoxes` `--nominivoxes` | no block voxes (minivoxes) | [models](../src/models/fun/_.c) |
| `--nosaves` | saves go to a temporary folder (`/tmp/zoxel_nosaves_<pid>`), real saves untouched | [saves](../src/saves/fun/terminal.c) |

The terrain sizes override `Far RD` from your settings for that run.

## Audio

| Flag | Effect | Source |
|---|---|---|
| `--nosound` `--nosounds` | no sound effects | [sounds](../src/sounds/fun/terminal.c) |
| `--nomusic` | no music | [musics](../src/musics/fun/_.c) |
| `--sounds` | sounds on again after `--background`, i.e. to listen to a test | [apps](../src/apps/fun/arguments.c) |

To record what the game plays: `SDL_AUDIO_DRIVER=disk`, the mix goes to `sdlaudio.raw` beside the binary
(stereo float32, 44100hz). See [audio.md](audio.md).

## Engine

| Flag | Effect | Source |
|---|---|---|
| `--fps <n>` | target frame rate | [_](../src/_/_.c) |
| `--singlethread` | runs systems on one thread, useful when hunting races | _ |
| `-p` `--profiler` | enables the flecs profiler (also `make profiler`) | _ |

## Networking

| Flag | Effect | Source |
|---|---|---|
| `-x` `--server` | server mode | [networking](../src/networking/fun/_.c) |
| `--client` | client mode | networking |
| `--online` / `--offline` | networking on / off | networking |
| `--lan` | connect to the local ip | networking |

## Tests and Debugging

Usually run through `bash bsh/tst.sh`, see [Onboarding](onboarding.md#6-tests).

| Flag | Effect | Source |
|---|---|---|
| `--profile` | logs every frame over 25ms: its systems, merges, observers, commands, and the components moved most | [core/tst](../src/core/tst/profile.c) |
| `--tst <name>` | runs one test, exits 0 on pass, 1 on fail | [core/tst](../src/core/tst/_.c) |
| `--tst list` | prints every test name and exits | core/tst |
| `--tst-errors <n>` | fails a test once it logs this many errors (default 100) | core/tst |
| `--tst-timescale <f>` | scales test timeouts, i.e. 2 for slow software rendering | core/tst |
| `--blackplague` | damages every npc every 5s, a stress test for deaths and cleanup | [skills](../src/skills/tst/plague.c) |

## Handy Combinations

```bash
# straight into a small world, quick to load
./zoxel-dev.bin --autostart --tiny --windowed

# play without touching your real saves
./zoxel-dev.bin --nosaves --autostart

# quiet, no npcs, for terrain work
./zoxel-dev.bin --autostart --nonpcs --nomusic --nosounds

# hunt a crash in npc deaths
gdb -batch -ex run -ex bt --args ./zoxel-dev.bin --autostart --blackplague
```

## Files the Game Uses

| Path | Holds |
|---|---|
| `~/.config/zoxel/settings.cfg` | settings (render distances, volume...) |
| `~/.local/share/zoxel/<realm>/` | a save: `seed.dat`, `player.dat`, edited `chunk_*.dat` files |
