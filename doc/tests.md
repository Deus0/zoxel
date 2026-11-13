# Tests

Zoxel tests run the real game: a test boots it, waits for the world to be ready, checks it, and exits.

```bash
bash bsh/tst.sh list                     # every test
bash bsh/tst.sh player_spawn             # build debug, run one
bash bsh/tst.sh all                      # run every test (before every commit)
bash bsh/tst.sh stream_npcs --repeat 5   # intermittent bugs
bash bsh/tst.sh <name> --gdb             # a backtrace on a crash
bash bsh/tst.sh <name> --nobuild         # rerun without building
```

- Exit code 0 is a pass. A failed run's log is kept: `bin/tst/<name>_failed_*.log`.
- Tests run hidden (`--background`), silent, with software rendering, capped memory and a timeout.
- On x86 (not the arm board), pick the egl device: `zox_tst_egl_device=0 bash bsh/tst.sh all`.
- The harness is [src/core/tst/_.c](../src/core/tst/_.c), the runner [bsh/tst.sh](../bsh/tst.sh).

## Writing one

A test is two functions in a module's `tst/` folder, registered in the module's import:

```c
// tst: my_thing, what it checks, in a line or a few
//  - run: bash bsh/tst.sh my_thing
byte zox_tst_my_thing_ready(ecs* world) {
    // called every frame: return 1 once the world is ready to check
    zox_tst_waiting = "the player's character";     // logged if it times out
    return zox_valid(dbg_player);
}

void zox_tst_my_thing_check(ecs* world) {
    zox_tst_check(1 + 1 == 2, "maths broke, got [%i]", 1 + 1);
}

void zox_tsts_my_module() {
    zox_tst_add("my_thing", zox_tst_my_thing_ready, zox_tst_my_thing_check, 30);
}
```

- **In game by default**: tests start with `--autostart`, straight into a new game.
- **Menus**: `zox_tst_set_setup(name, NULL, 1)` runs through the main menu instead (no game).
  Pure logic tests (composing, octrees, mixing) use it too, they're quicker.
- **Exits**: `zox_tst_set_exits(name)` passes when the app closes itself (exit buttons).

## Patterns

- **Stages**: a test that does several things keeps a stage and its frames, one step per stage
  (`tst resume_no_punch`, `tst quest_dialogue`). Wait a few frames after a change: commands are deferred.
- **Deterministic**: if what you need is random (a quest giver nearby), make it, don't wait for it
  (`tst quest_dialogue` makes an npc a quest giver).
- **The fake mouse**: [src/ui/tst/helpers.c](../src/ui/tst/helpers.c), `zox_tst_mouse(position, pressed)`,
  `zox_tst_find_click`, `zox_tst_ui_pointer_position`. Press and release on different frames.
- **Listening**: tests run silent. A test can turn sounds on (`nosounds = 0`) to check sounds spawn, nothing
  plays. To hear one: `--sounds`, to record one: `SDL_AUDIO_DRIVER=disk` (see [audio.md](audio.md)).
- **Files to look at**: write a `.wav` or a screenshot to `bin/tst/` (`song.wav`, `voices.wav`, `screenshot.ppm`).

## When you fix a bug

Add a test that would have caught it, then **check it does**: put the bug back, run the test, see it fail.
(`tst node_graphs` fails with the old exclusive `NodeLink`.)

## The tests

| Area | Tests |
|---|---|
| Lints (scripts, no game) | `lint_systems`: system names, signatures, timers (bsh/lint_systems.py) |
| Core | `hashmaps` `links_api` `links_cleanup` `children` `node_graphs` `spawn_queue` |
| Maths | `maths_vectors` `maths_quaternions` |
| Physics | `physics_steps` `physics2_motion` `physics2_rotation` `physics3_motion` `physics3_rotation` `terrain_collisions` (each also `_10fps` `_30fps`), see [physics.md](physics.md) |
| Octrees and chunks | `octree_*` `chunk_positions` `chunk_io` `chunk_sides` `vox_mesh_grid` `pipeline_syncs` `tilemap` |
| Terrain | `biome_heights` `biome_map` |
| Rendering | `lines_spawn` `lines_render` `skeleton_render` `vox_instance_render` `terrain_render` `block_vox_light_sampling` `block_vox_lights` `screenshot` |
| Sound and music | `sound_mixer` `sound_formats` `sound_instruments` `sound_balance` `voice_sounds` `music_*` |
| Text and ui | `text_data` `text_order` `font_glyphs` `menu_flow` `mouse_click` `ui_leaks` `tooltips` `options_toggle` `scroll_wheel` `list_order` `ui_layout` `exit_menu` `exit_game` |
| Game | `load_games` `player_spawn` `continue_game` `end_game_cleanup` `stream_npcs` `parts` `action_wheel` `item_split` `resume_no_punch` `pickup_pop` `editor` |
| Quests | `quest_tracking` `quest_dialogue` |
| Performance | `perf_stream` `perf_fly` `mass_spawn` `mass_delete` `physics2_perf` `physics3_perf` |

The full list with descriptions: the comment at the top of each test, or `bash bsh/tst.sh list`.
