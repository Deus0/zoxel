# Test Results

The latest test times, by module, to compare against (todo.md: optimize and reduce test times). Each run alone
(`bash bsh/tst.sh <name> --nobuild`, debug build, llvmpipe, deusmini), nothing else running. Times from
`tst/<name>.log`: boot is start to `[tst] running` (~0.3s for every test), own is from there to the result.

## src/_ (2026-11-24)
36 tests, all pass. Every one but these is under 1s with its boot:

| Test | Own time | Notes |
|---|---|---|
| pipeline_syncs | 17.4s | waits for a player in the whole game: flecs counts merges only between active systems, so it needs npcs, towns and terrain running (the test level or menus would hide them) |
| flecs_event_scaling | 4.0s (was 10.8s) | 10000 events a frame timed over 8 frames, not 40; entities bulk spawned |
| flecs_wildcard_remove | 0.7s | |

## transforms, apps/sdl, colors, nodes, cameras (2026-11-24)
23 tests, all pass, all under 1s own time. sdl_vulkan_surface is vulkan only (`--vulkan --visible`).

| Test | Own time | Notes |
|---|---|---|
| sdl_window_mode, sdl_window_restore | 0.45s (was 1.9s) | each toggle waits for the window's mode, then 4 frames, not 30 frames |
| camera_viewport_spawn | 0.8s | |

## rendering, sounds, animations, textures, geometry, musics, ui (2026-11-24)
36 tests, all pass.

| Test | Own time | Notes |
|---|---|---|
| gpu_restore_game | 17.8s | the whole game's gpu objects restored (block voxes, npcs, towns): kept in the game, gpu_restore is the quick one |
| skeleton_render | 4.5s (was 13.6s) | in the void: the player's skeleton, no npcs to wait for |
| vox_instance_render | 7.1s | in the game: the void has no instanced renderers |
| terrain_render | 6.4s | the test level (a world of its own is in controllers, above rendering) |
| music_theme | 4.1s (was 7.0s) | the test mixer synthesizes a repeated note once; the realm song's notes are nearly all different |
| sound_balance, music_compose | 3.4s, 2.6s | mixing realm songs (few repeated notes) |
| text_order | 1.7s | |

## Every realm (2026-11-24)
Realm generation takes ~3.5s in a debug build, ~3s of it wave function collapse (the 4 chook and 4 pengu critter
models, one after another in an observer on the main thread, models/eve/wfc.c): every test with a game or the void
pays it. wfc.c is built -O2 in debug builds too now (3.2s to 2.6s); on threads it'd be a fraction (asked, todo.md).

## Meshing speed, perf_meshing (2026-11-25)
The cpu meshers on the same voxels every run (src/chunks/chunks3/tst/perf_meshing.c): ms a mesh, the fastest of 5
rounds, runs within 1% of each other. The baseline for doc/plans/chunks_refactor.md: each step's run compared to this.
Textured is a 16³ terrain chunk and its 6 neighbours (sides + mesh), colored a 32³ model (both walkers).

| Mesh (lod depth) | Faces | Debug ms | Release ms |
|---|---|---|---|
| textured 4 | 1206 | 1.95 (sides 1.75, mesh 0.20) | 0.274 (sides 0.228, mesh 0.046) |
| textured 3 | 202 | 0.266 (sides 0.234) | 0.036 (sides 0.028) |
| textured 2 | 81 | 0.047 | 0.0067 |
| colored grid 5 | 10022 | 6.10 | 0.900 |
| colored grid 4 | 5566 | 2.06 | 0.273 |
| colored grid 3 | 1798 | 0.49 | 0.068 |
| colored walk 5 | 10022 | 24.1 | 4.35 |
| colored walk 4 | 5566 | 5.21 | 0.898 |
| colored walk 3 | 1798 | 0.89 | 0.160 |

- A textured chunk's sides (`build_sides_dig`) are ~85% of its cpu cost, debug and release: what the plan's phase 2
  (faces from the voxel grid, no SidesOctree) replaces
- The pyramid grid is 4 to 5 times the octree walk at full detail: the walk to keep for colored is the grid's
