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
Terrain is a chunk and its 6 neighbours at its lod, two ways: sides (SidesOctree then the mesh, the game's now) and
grid (sys/build_grid.c, faces by the gpu mesher's rule, the same faces checked). * its neighbours at full detail, a
lod border on every side (the worst case). Models are a 32³ ball, both walkers.

| Mesh | Faces | Debug sides | Debug grid | Release sides | Release grid |
|---|---|---|---|---|---|
| terrain 16³ | 1206 | 1.948 | 0.255 (7.7x) | 0.281 | 0.091 (3.1x) |
| terrain 8³ | 137 | 0.253 | 0.041 (6.1x) | 0.034 | 0.017 (1.9x) |
| terrain 4³ | 31 | 0.035 | 0.009 (3.9x) | 0.0047 | 0.0027 (1.7x) |
| terrain 2³ | 4 | 0.0045 | 0.0016 (2.9x) | 0.0006 | 0.0004 (1.5x) |
| terrain 1³ | 1 | 0.0005 | 0.0003 (1.5x) | 0.0001 | 0.0001 (1.1x) |
| terrain 8³ * | 202 | 0.267 | 0.096 (2.8x) | 0.036 | 0.025 (1.4x) |
| terrain 4³ * | 81 | 0.047 | 0.052 (0.9x) | 0.0070 | 0.0086 (0.8x) |

| A world, far 16 (6223 chunks: 16³ 315, 8³ 1036, 4³ 1708, 2³ 2492, 1³ 672) | Sides | Grid |
|---|---|---|
| meshed once, debug | 947.5ms | 142.7ms (6.6x) |
| meshed once, release | 132.8ms | 52.4ms (2.5x) |

| Model | Faces | Debug pyramid | Debug walk | Release pyramid | Release walk |
|---|---|---|---|---|---|
| 32³ | 10022 | 6.10 | 24.1 | 0.900 | 4.35 |
| 16³ | 5566 | 2.06 | 5.21 | 0.272 | 0.898 |
| 8³ | 1798 | 0.49 | 0.89 | 0.068 | 0.160 |

Models on the gpu (2026-11-26, release, `perf_meshing --release --gpu`, the 780M): prepare is the main thread's part
(grids, the count), write the dispatch waited for; the same faces as the cpu walk. Release cpu the same run: pyramid
0.912, 0.253, 0.060.

| Model | Faces | Prepare | Write | Total |
|---|---|---|---|---|
| 32³ | 10022 | 0.295 | 0.301 | 0.597 |
| 16³ | 5566 | 0.083 | 0.161 | 0.244 |
| 8³ | 1798 | 0.018 | 0.117 | 0.135 |

- merged (greedy, the same run): the cpu's pass alone 1.03, 0.44, 0.12 (9540, 4866, 1210 faces: the test ball's holes
  and 3 colours merge little, a humanoid 20% fewer); on the gpu 0.98 total, 0.65 of it the gpu's (0.33 unmerged)
- small models are latency bound when waited for (the game doesn't wait); software rendering's write is 2.5x slower
- a function used by both meshers changes how gcc compiles the cpu walk: `move_position` called from the gpu's face
  count made the pyramid 65% slower (0.91 to 1.51ms, release); the gpu's count has its own directions

- A textured chunk's sides (`build_sides_dig`) are 80 to 90% of its cpu cost: the grid replaces them (plan phase 2)
- 78% of a far 16 world's chunks are 4³ or smaller: the lower lods are most of the meshing
- The colored pyramid grid is 4 to 5 times the octree walk at full detail: the walk to keep is the grid's

## Grid meshing in the game, perf_terrain (2026-11-25)
`bash bsh/tst.sh perf_terrain --gpu` (far 8, debug, cpu meshing, the 780M), the grid (now) against `--sides_meshing`:

| | Sides | Grid |
|---|---|---|
| load settles | 11.12s | 6.82s |
| load fps average (1% low) | 23.1 (9.5) | 27.5 (9.6) |
| stream steps settle, average (slowest) | 8.60s (9.99s) | 7.01s (7.78s) |
| stream fps average (1% low) | 24.8 (13.8) | 33.0 (13.1) |
| stream frames under 30 fps | 683 of 936 | 311 of 921 |
| idle fps average (1% low) | 117.5 (102.3) | 119.8 (96.9) |

stream_seams (36910 borders, 7022 between lods), chunk_mesh_flicker, lod_light_flash: no seams, blanks or flashes
either way.
