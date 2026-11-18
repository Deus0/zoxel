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

- Exit code 0 is a pass. A failed run's log is kept: `tst/<name>_failed_*.log`.
- Tests run hidden (`--background`), silent, with software rendering, capped memory and a timeout.
- tst.sh builds `--debug --logs`. Testing a `--profiler` build with `--nobuild` adds merges (pipeline_syncs fails on it)
- Tests run on `--tiny` terrain, ones in `medium_tests` (bsh/tst.sh) on `--medium` (far lods to compare).
- Tests render in software (mesa llvmpipe, offscreen). The egl device is picked by machine: 0 on x86, 2 on the
  arm board. Override it with `zox_tst_egl_device=<n>` ("Invalid EGL device is requested" means a wrong one).
- Tests use their own settings (defaults, i.e. Block Res 4). To run like real play, copy real ones in:
  `zox_tst_settings=$HOME/.config/zoxel/settings.cfg bash bsh/tst.sh clothes` (a finer Block Res doubles every model).
  Load tests also take `zox_tst_save=<realm folder>`, a copy of a real save.
- `make run` is the release build, tests the dev one: a release-only bug needs the release binary (`make prerelease`).
- The harness is [src/_/tst/_.c](../src/_/tst/_.c), the runner [bsh/tst.sh](../bsh/tst.sh).

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
- **Files to look at**: write a `.wav` or a screenshot to `tst/` with `zox_tst_path("song.wav")` (makes sub folders,
  `zox_tst_path("models/cokie.ppm")`), never a hardcoded path: the game runs from `bin/<build>/`, tst.sh passes the folder.
- **A screenshot of a game**: `bash bsh/tst.sh screenshot` (`tst/screenshot.ppm`), `--xr-sim` for the XR simulator's
  two eyes side by side (a 1440x720 window, each eye square).
- **Looking at a character**: `bash bsh/body.sh` (your settings, i.e. Block Res 5), then `tst/body_views.png`: a
  dressed character in the void from 4 viewports (src/controllers/tst/body_views.c).
- **Looking at a model**: `bash bsh/model.sh <name>` (iterating a blueprint: edit, run, look at
  `tst/models/<name>.png`, ~45s with the build, ~10s `--nobuild`), or `zox_tst_model=<name> bash bsh/tst.sh model_views`, then `tst/model_views.ppm` (kept per model in `tst/models/<name>.ppm`): a realm
  model from 4 viewports on gray (`spawn_viewport_camera`, `CameraFilter`, see [src/models/tst/model_views.c](../src/models/tst/model_views.c)).

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
| Octrees and chunks | `octree_*` `chunk_positions` `chunk_io` `block_table` `block_table_remap` `chunk_sides` `vox_mesh_grid` `pipeline_syncs` `tilemap` |
| Terrain | `biome_heights` `biome_map` `region_place` `region_place_prior` `region_zones` `town_homes` `home_roofs` `home_glow` `glow_light` `town_maps` `town_maps_empty` `vegetation_trees` `vegetation_cover` `vegetation_chunks` |
| Textures | `texture_noise` `texture_outline` |
| Lights | `smooth_lights` `lod_lights` `lod_chunk_lights` `block_vox_lights` `block_vox_light_sampling` |
| Rendering | `lines_spawn` `lines_render` `skeleton_render` `vox_instance_render` `terrain_render` `block_vox_light_sampling` `block_vox_lights` `screenshot` `model_views` `body_views` |
| Models | `model_nodes` (fill, box, paint, sprinkle on voxels) `model_shapes` (realm models: one piece, mirrored, head's neck and eyes, chest without a pinch, cookie chips and crumbs) |
| Cameras | `camera_viewports` `camera_projection` `camera_orthographic` `camera_look_at` `camera_viewport_spawn` `camera_split_screen` `camera_downscale` `camera_frustum` `camera_filter` |
| Sound and music | `sound_mixer` `sound_formats` `sound_instruments` `sound_balance` `voice_sounds` `hit_sounds` `hit_sound_links` `music_*` |
| Body and gear | `parts` `clothes` `npc_skeletons` |
| Text and ui | `text_data` `text_order` `font_glyphs` `font_fill` `font_glyphs_fill` `texture_lines` `menu_flow` `mouse_click` `ui_leaks` `tooltips` `options_toggle` `scroll_wheel` `list_order` `ui_layout` `exit_menu` `exit_game` |
| Game | `load_games` `player_spawn` `continue_game` `end_game_cleanup` `stream_npcs` `npc_attacks` `population` `region_population` `npc_leash` `player_respawn` `footsteps` `arms_in_step` `parts` `action_wheel` `item_split` `resume_no_punch` `pickup_pop` `editor` |
| Quests | `quest_tracking` `quest_dialogue` |
| Performance | `perf_stream` `perf_fly` `mass_spawn` `mass_delete` `physics2_perf` `physics3_perf` |

Perf runs: compare on the same land and machine state. `zox_tst_seed=666` gives new games a fixed seed (random land
swung perf_fly 319 to 851 frames), `--stress --far 8` the user's far distance, `--gpu` the real gpu (stop ollama first,
it shares it), e.g. `zox_tst_seed=666 bash bsh/tst.sh perf_fly --gpu --stress --far 8`. perf_fly logs the queue, its
slots by kind and the near lod latency (doc/done/streaming.md).

The full list with descriptions: the comment at the top of each test, or `bash bsh/tst.sh list`.

## Looking at blocks and models
- A model or block model: `bash bsh/model.sh <name>` (tst model_views, 4 views on gray) -> tst/models/<name>.png
- An animated block (water): `bash bsh/tst.sh block_frames` -> tst/blocks/<block>_frames.ppm, its frames in a row,
  its first tiled 2 x 2 (seams) and its last then first (the loop); checks they change, loop and tile
- Water in the world: `bash bsh/tst.sh sea_level` -> tst/sea.ppm and sea_later.ppm, looking down at open water
- The world without a player: zox_tst_world_ready (src/controllers/tst/world_only.c) makes a realm and terrain on
  the main menu (register the test with zox_tst_set_setup(name, NULL, 1)); nothing streams until the test adds a
  streamer: spawn_streamer, or a camera (spawn_viewport_camera, prefab_add_streamer, StreamLink to the terrain),
  with StreamerLevel 1 (0 streams regions and towns only); for a picture, nohud and the menu view's RenderDisabled
  (tst sea_level: one camera; tst two_streamers: two streamers, the terrain streams round each)
