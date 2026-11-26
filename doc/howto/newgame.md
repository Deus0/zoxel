# How to make a new game

A game is a folder in [gam](../../gam) built on the engine in `src/`. The build picks one: `bash bsh/build.sh <game>`
passes `-Dzox_game=<game>`, and [main.c](../../src/main.c) includes `gam/<game>/_.c` after every engine module, so a
game can use anything in `src/`. Each game builds its own binary, `bin/<game>_<os>_<arch>_..`.

The games so far, smallest first, to copy from:

| Game | What it shows |
|---|---|
| [zixel](../../gam/zixel) | the least a game needs: a name, a mode, sky colours, its boot menu |
| [zeland](../../gam/zeland) | a 3D game without voxels (the poly game mode), its own way into play, a test |
| [zedit](../../gam/zedit) | no main menu: boots straight into the editor's windows |
| [zoxel](../../gam/zoxel) | the whole rpg: settings, realm content (blocks, items, skills, quests), music |

The engine is still shaped round zoxel: this page says what a new game has to work round today. Update it as the
engine improves (doc/plans/editor.md "Refactored games", doc/plans/poly_terrains.md).

## 1. The folder

```text
gam/<game>/
    _.c         the game's module: settings and hooks, import_zoxgame
    eve/_.c     its observers: what it does when the game's or a player's state changes
    tst/_.c     its tests (only built with zox_tests)
    res/        its own resources, packaged with the game (optional, the engine's res/ is shared)
```

No `#include` of anything outside the folder: the engine is already in (one translation unit, doc/onboarding.md).

## 2. `_.c`

Every game defines `import_zoxgame`. Set the game's settings here, before the game spawns:

```c
// Game Directory
// <game>: one line on what it is
#include "eve/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_zoxgame(ecs* world) {
    zox_module(zoxgame);
    game_name = "<game>";
    zox_game_type = zox_game_mode_poly;     // see Game modes
    menu_sky_color = (color_rgb) { 120, 170, 220 };
    menu_sky_bottom_color = (color_rgb) { 200, 220, 235 };
    game_sky_color = (color_rgb) { 110, 165, 225 };
    game_sky_bottom_color = (color_rgb) { 215, 230, 240 };
    add_hook_on_boot(spawn_weather);        // the skybox (else "Game has no skybox" errors)
    zox_events_<game>(world);
#ifdef zox_tests
    zox_tsts_<game>(world);
#endif
}
```

Other settings a game may set are globals of the engine's modules (`set/` folders, doc/tech/settings.md): zoxel's
`initialize_zoxel_settings` (gam/zoxel/set) is the long example: ui colours, the camera mode, movement speeds.

## 3. Game modes

`zox_game_type` (src/games/dat/modes.c) picks what a game starts with:

| Mode | Starts with | Players |
|---|---|---|
| `zox_game_mode_3D` | the voxel terrain, streamed, and a character3 for each player | `Player3` |
| `zox_game_mode_2D` | a 2D grid and a character2 | `Player2` |
| `zox_game_mode_poly` | nothing yet: the game spawns its own world (a poly terrain, doc/plans/poly_terrains.md) | neither |
| `zox_game_mode_editor` | the default, no game play | |

In a poly game the voxel start (controllers3's `player_state_game_begin`) skips its players, so the game takes each
player from loading into play itself, see the next section.

## 4. `eve/_.c`: the game's states

The game entity's `GameState` and each player's `PlayerState` change through a state machine (src/games/dat/states.c,
src/players/sta/_.c); each change adds core `Dirty`, and games observe it:

```c
// the game's state changed: at boot, the start menu
void game_state_<game>(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GameState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(GameState, state);
        if (state->value == zox_game_boot) {
            spawn_boot_menu(world);
        }
    }
}

// a player loading into a game: place its camera, then play (poly games, see gam/zeland/eve)
void player_state_<game>(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        if (state->value != zox_player_state_loading) {
            continue;
        }
        // .. the camera, the character
        zox_setv(e, PlayerStateTarget, zox_player_state_play_trigger);
    }
}

void zox_events_<game>(ecs* world) {
    zox_on_add(game_state_<game>, [in] games.GameState, [none] games.Game, [none] core.Dirty);
    zox_on_add(player_state_<game>, [in] players.PlayerState, [none] players.Player, [none] core.Dirty);
}
```

- Without `spawn_boot_menu` at `zox_game_boot` there's no main menu (zedit spawns its editor windows instead).
- Play goes: main menu, the realm generates (`GenerateRealm`), the terrain spawns (by mode), players load, then
  `zox_player_state_play_trigger`, `play_begin` (the game's ui spawns) and `playing`.
- The game's ui at `play_begin` spawns only what the player has: an actionbar, stats and a quest tracker need a
  character (`CharacterLink`), the minimap a voxel terrain. A game without them gets none, with no errors.

## 5. Tests

Tests go in `tst/_.c`, registered with `zox_tst_add` like any module's (doc/engine/tests.md). Tests run with
`--autostart`, so a test starts in a new game. gam/zeland/tst is a short example that checks the game plays.

```bash
zox_tst_game=<game> bash bsh/tst.sh <test>      # builds that game with tests, runs one
```

Every game should have a boot test (zedit_boot, zeland_boot): it catches engine changes that assume zoxel.

## 6. Build and run

```bash
bash bsh/build.sh <game> --run          # release
bash bsh/build.sh <game> --debug --logs --run
```

The other targets (windows, android, web) take the game's name the same way (doc/engine/devops.md); android also
wants `gam/<game>/android/res` icons (bsh/android_icons.sh).

## Today's limits

- A 3D game without voxels has no character yet: character3 still needs the voxel terrain (poly terrains plan,
  step "poly terrain physics").
- Realm generation runs for every game: zoxel's realm content (blocks, items) is game code in gam/zoxel/sys, a new
  game's realm starts without it.
- Settings are globals: a game sets them in `import_zoxgame`, not per realm.
