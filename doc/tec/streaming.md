# Terrain Streaming and LODs

Why terrain loads slowly and LODs lag behind when flying, what limits it, and a plan to speed it up. There's no frame
lag: the work is throttled, not slow. Written 2026-11-14 from reading the code, a simulation of one tunk crossing, and a
timed `perf_stream` run (numbers below).

Code: [src/terrain/sys/stream.c](../../src/terrain/sys/stream.c) (the queue),
[src/streaming/com/queue.c](../../src/streaming/com/queue.c) (`TerrainSpawnQueue`),
[src/terrain/sys/render_depth.c](../../src/terrain/sys/render_depth.c) (`chunk_lod_system`),
[src/chunks/chunks3/sys](../../src/chunks/chunks3/sys) (meshes: `mesh_spawn.c`, `sides.c`, `build_textured.c`, `transition.c`),
[src/chunks/chunks3/fun/lods.c](../../src/chunks/chunks3/fun/lods.c) (distance to LOD),
[src/controllers/tst/perf.c](../../src/controllers/tst/perf.c) (`perf_stream`).

## Summary

1. **The stream queue is the bottleneck.** It handles 60 tunks a second, at most 2 a frame, and every crossing of a tunk
   queues the whole `(2·far+1)²` square. At Far 8 one crossing costs 145 queue slots: 2.4s at 60fps, longer at lower
   fps. Fly faster than one tunk per ~2.4s and the backlog only grows.
2. **Most of that budget isn't LOD work.** 43% (Far 8) to 66% (Far 16) of the slots are distance-only updates that
   change no LOD, and they're processed in the same 60/s line as the LOD changes and spawns that matter.
3. **Under sustained flight the queue turns first-in-first-out.** An update waiting over 2s goes before closer ones
   (`stream_update_max_wait`), so once the backlog is 2s deep, the tunks near the player wait behind old far ones.
4. **Downstream caps count per system call, not per frame.** Terrain mesh builds (`build_textured.c`), smooth lights
   and landfill stop after 1-5 entities per call (`max_process`). How much runs a frame depends on the table count
   and worker split, not on time. `sides.c` and `vox_lods.c` already use a time budget (`zox_sys_budget`) instead.
5. **The LOD swap is a state machine stepping once a frame**, with two 0.1s timers in the transition that restart
   while the mesh or its lights are busy.
6. **The measurements don't see the queue.** `perf_stream` calls a step settled while hundreds of updates are still
   queued, and runs on `--tiny` (Far 3) by default, where none of this shows.

## How it works now

From the player moving to a new LOD on screen:

1. `stream_position_system`: the camera (a `Streamer`) entered a new tunk → `StreamDirty`.
2. `terrain_stream_system`: queues every tunk position in the `(2·far+1)²` square around it (`add_or_set_TerrainSpawnQueue`,
   one entry per position, a newer one replaces it but keeps its `queued` time).
3. `terrain_stream_queue_system` (main thread, `zoxp_spawn`): credit of `stream_updates_per_second` (60) × delta, capped at
   `stream_updates_max_per_frame` (2). Each pick (`remove_next_TerrainSpawnQueue`, an O(n) scan): out of range first, else
   the oldest if it waited over `stream_update_max_wait` (2s), else the closest. Per pick:
   - out of range: deletes the tunk and its chunks (doesn't use a slot)
   - same distance: nothing (doesn't use a slot)
   - new distance: sets `RenderDistance` + dirty on the tunk and its `2·vertical+1` chunks (**uses a slot**)
   - new LOD (`camera_distance_to_terrain_render_depth`): also `RenderDepth`, `TunkLod` + `GenerateTunk` if it went up,
     each chunk `ChunkLodDirty = start`, `BuildDisabled` on its active mesh, its lights cleared if the depth went up
   - no tunk yet: spawns its region if needed, the tunk and its chunks (**uses a slot**)
4. `chunk_lod_system`: `ChunkLodDirty` generate → generating, and `GenerateChunk` if the depth went up.
5. Generation: landfill (waits for its tunk's `GenerateTunk`, the height and biome maps), vegetation, towns,
   sunlight. Then `chunk_lod_system` generating → spawn once no generation tags are left.
6. `chunk_mesh_spawn2_system` (main thread): spawns the mesh for the new depth → toggle.
7. Sides (`chunk_sides_system`, 3ms a worker a frame), the mesh (`chunk3_textured_build_system`), lights
   (`build_smooth_lights_system`).
8. `chunk_mesh_transition_system` (only once `!GenerateChunk`, `!BuildChunkSides`): one step a frame. Link the
   preparing mesh, disable it, enable building and wait 0.1s, disable the old mesh's building and wait 0.1s, then wait
   until the new mesh and its lights aren't busy (each busy frame restarts the 0.1s), then swap.

## Measurements

### One tunk crossing, by far distance

Simulated with the game's own distance (`int2_distance`, rounded down) and LOD bands
(`camera_distance_to_render_depth`, near 3). Slots are what the 60/s queue budget pays for. Each LOD change is
`2·vertical+1` chunk LOD changes (×7 at vertical 3).

| Far | terrain depth | tunks in range | queued | LOD changes (up / down) | distance only | spawns | slots | at 60/s |
|---|---|---|---|---|---|---|---|---|
| 8 | 4 | 249 | 289 | 66 (33 / 33) | 62 | 17 | 145 | 2.4s |
| 8 | 5 | 249 | 289 | 110 (55 / 55) | 18 | 17 | 145 | 2.4s |
| 12 | 4 | 517 | 625 | 78 (39 / 39) | 210 | 25 | 313 | 5.2s |
| 16 | 4 | 889 | 1089 | 152 (76 / 76) | 360 | 33 | 545 | 9.1s |
| 32 | 4 | 3405 | 4225 | 248 (124 / 124) | 1800 | 65 | 2113 | 35.2s |

The user's settings are Far 8, Near 3, Vertical 3. `terrain_depth` is 4 by default (`chunks3/set/_.c`).

### A timed run

`zox_tst_timeout=500 bash bsh/tst.sh perf_stream --stress --large` (Far 12, vertical 4) on the Khadas with software GL
(llvmpipe, roughly 10-15fps), logging the queue and pipeline once a second. Compare ratios, not seconds, on a faster
machine.

- **Initial load:** the queue held ~1008 tunks and drained ~26-34 a second (the 2-a-frame cap at low fps, not 60/s),
  so spawning took ~40s and the player spawned after 47.8s. `GenerateChunk` stayed around 40 throughout: generation
  kept up with what the queue let through.
- **Step 0 "settled" in 0.6s with 625 updates still queued.** The test only checks generation, sides and meshes.
- **Step 1:** the queue (625) drained in ~18s. Meanwhile `BuildMesh` grew to ~480 and `BuildDisabled` to ~960, and the
  step failed after 20s with 474 meshes "still busy". `ChunkLodDirty` was back to 1-2, so the swaps had finished: the
  busy meshes were old, disabled meshes with `BuildMesh` that will never build (see below).

## Where the time goes, in order

### 1. The queue budget (biggest)

- **Count-based, not cost-based.** A distance-only update (7 `RenderDistance` sets) and a tunk spawn (a tunk, 7 chunks,
  maybe a region) each take one of 60 slots.
- **The 2-a-frame cap makes it fps-bound again.** Below 30fps it's `2 × fps`, and heavy streaming lowers the fps.
- **Distance-only updates fill most of it** (table above). They matter (`chunk_entities_lod_system`, vegetation, npcs
  read `RenderDistance`) but are cheap, and could be applied to every queued tunk at once.
- **Re-queueing the whole square** each crossing is fine for correctness, and only the changed ones cost a slot, but the
  picks are O(n) scans over the whole queue (~1000 entries at Far 16), 2 a frame: cheap now, not if the budget goes up.

### 2. The 2s "oldest first" rule

`remove_next_TerrainSpawnQueue` picks the oldest entry once it has waited over 2s. A re-queued position keeps its old
`queued` time, so while flying, the far, old, distance-only entries keep winning over new LOD increases next to the player.
This is the "LODs update very slowly" feel: the near ones wait for the whole backlog.

### 3. Per-call caps downstream

`max_process` in these systems counts per system call (one table, and one worker's slice of it), then `break`s:

| System | File | Cap |
|---|---|---|
| `chunk3_textured_build_system` (terrain meshes) | `chunks/chunks3/sys/build_textured.c:158` | 2 per call |
| `build_smooth_lights_system` | `lights/lights3/sys/build_smooth.c:175` | 2 per call |
| `chunk_colors_build_system` | `chunks/chunks3/sys/build_colored.c:424` | 2 per call |
| `landfill_chunk_system` | `heights/sys/landfill.c:18` | `terrain_depth` + 1 per call |

Throughput then depends on how many tables hold those entities and how flecs splits them among workers, which changes
as tags fragment. `zox_sys_budget` (`_/flecs/mcr/systems.c`, used by `sides.c` and `vox_lods.c`) gives each worker a few ms
a frame instead. It's time based, so it follows the machine and still prevents lag spikes. The transition waits on both
the mesh and its lights, so these two caps gate every swap.

### 4. The swap's state machine

- `ChunkLodDirty`: start (1) = generate (1) → generating (2) → spawn (3) → toggle (4) → removed. Each step is a different
  system, so at least a frame each, more where a system runs before the one that set it.
- `chunk_mesh_transition_system` adds about 4 more frames (one action and a `continue` each) and two 0.1s timers, and
  restarts the timer on every busy frame.
- A depth decrease (behind the player) skips generation but still walks the whole chain and rebuilds a mesh.

### 5. Tunk maps on every LOD increase

An LOD increase sets `GenerateTunk`, which regenerates the tunk's height and biome maps, and landfill waits for it
(`heights/sys/landfill.c:72`). doc/todo.md already notes `height_map_system` lagging and tunk generation needing LODs.

### 6. Old meshes collecting `BuildMesh`

When a chunk's LOD changes, its old active mesh gets `BuildDisabled`. Rebuild triggers (lights, neighbours) still add
`BuildMesh` to it, and nothing removes it. In the run, ~480 meshes waited like this after one step. They don't hold up
the swap, but they're pending work, a busy count `perf_stream` fails on, and a rebuild when that LOD comes back.

## Measuring it first

The test has to see the problem before anything is changed.

- `perf_stream` settles only on generation, sides and meshes (`zox_tst_perf_busy`). Also wait for the terrain's
  `TerrainSpawnQueue` to be empty and no `ChunkLodDirty`, and don't count meshes with `BuildDisabled`.
- Tests run on `--tiny` (Far 3), where a crossing is a handful of slots. `bsh/tst.sh` adds `--tiny` after the extra
  arguments, so `--large` alone loses. Use `--stress --large`, or add `perf_stream` to a list like `medium_tests` that
  sets its own size.
- Add a flying test (doc/todo.md mentions `perf_fly`): move the player at a steady speed across ~10 tunks and log, per
  second, the queue size, the slots used by kind (LOD, distance only, spawn), and, most important, **how long the
  tunks within 2 of the player take from crossing to their new LOD's mesh being active**. That last number is what
  the user feels.
- Log those as `[tst] [metric]` lines so runs before and after each change compare.

## Plan

Each step is its own commit, measured with the test above before and after.

1. **Measure** (above): `perf_stream` waits for the queue and ignores disabled meshes; a steady flying test with the
   "near LOD latency" metric.
2. **Take distance-only updates out of the budget.** Apply `RenderDistance` to every queued tunk whose LOD didn't change,
   all at once or with a large separate budget. Only LOD changes and spawns use the throttled slots.
3. **Fix the priority.** Closest first, LOD increases near the player before anything else. Age entries without
   letting old far ones jump the queue, i.e. reset `queued` when a position is re-queued, or bound the "oldest" rule to
   updates that change LOD.
4. **Budget by time.** Replace the slot count and the 2-a-frame cap with a few ms a frame (`zox_sys_budget` style), so
   a fast machine streams faster and a slow frame still doesn't spike.
5. **Replace `max_process` with `zox_sys_budget`** in `build_textured.c`, `build_smooth.c`, `build_colored.c` and
   `landfill.c`.
6. **Shorten the swap.** Drop or shorten the 0.1s transition timers (or end them on an event, not a restarting
   timer), let a step fall through to the next in the same frame where it can, and give start and generate their own
   values if both are needed.
7. **Clear `BuildMesh` on disabled meshes** when `BuildDisabled` is added (rebuild when re-enabled), or don't trigger
   builds on them.
8. **Later:** tunk map LODs (5), and the existing doc/todo.md terrain items (priority by LOD, update groups, the
   flicker bugs) once the pipeline is measurable.

## Smaller things noticed

- `src/tunks/sys/stream_lod.c` is a commented-out older copy of the queue's LOD update (`tunk_lod_system`), using the
  `max` distance where the queue uses Euclidean. Delete it, or note which is meant.
- `src/chunks/chunks3/sys/wait.c` is mostly commented-out queue and grouping code.
- `zox_chunk_lod_dirty_start` and `zox_chunk_lod_dirty_generate` are both 1 (`chunks3/com/_.c:53`).
- `fudge_frustum_extents = 1.1f; // ehh?? we need this??` (`streaming/set/_.c`).

## Questions for the user

- Should distance-only updates still reach chunks right away (npc and vegetation LODs), or can they lag behind LOD
  changes?
- ~~Is the 0.1s transition timer there to hide a flicker?~~ Yes (the user, 2026-11-14): there were real flicker bugs, and
  much of the transition (timers, waiting on lights, `BuildDisabled`) was fudged to hide them. Treat it as replaceable:
  write a test that catches the flicker first (i.e. a chunk with no active, built, lit mesh for a frame while it swaps),
  then try simpler swaps against it.
- What far distance should it be tuned for? Far 8 is your setting, the default is 16, the slider goes to 64.
