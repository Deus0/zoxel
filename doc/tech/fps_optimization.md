# FPS Optimization: frame drops while streaming

The game plays in VR: a frame over 11ms (under 90 fps) is felt, dips cause motion sickness. While land streams in,
frames drop well under that. This lists what's known (2026-11-25) and what to do, in order.

## How to measure

- `zox_tst_seed=666 bash bsh/tst.sh perf_terrain --release --gpu` (far 8, terrain alone). Read the stream block's
  1% low and its frames over 11.1ms first, the average last: an average up with the lows flat isn't a win.
- Add `--profile-ms 11 --profile` to log every frame over 11ms with its slowest systems, and the slowest frames and
  systems of the stream (src/_/tst/profile.c).
- `--profile` changes the timings (a profiled sides run settled steps in 0.86s at 201 fps, unprofiled ones 1.8s at
  100): profile to find what's slow, measure without it. Compare unprofiled runs, alternating the two things
  compared, several runs each, the same seed, nothing else running (other sessions' builds, ollama).
- Idle (standing still) fps is a weak signal for streaming work: flecs table and query changes move it, and the
  machine does (the same code drew 660 fps on 2026-11-20, 455 on 2026-11-25).
- More in doc/engine/metrics.md (numbers by backend) and doc/engine/tests.md (the perf tests, their scale).

## What's known

### Grid meshing cuts the drops (2026-11-25, fd2b0145e)

perf_terrain, release, far 8, seed 666, 4 runs each alternated, unprofiled, cpu meshing:

| Streaming | Sides (`--sides_meshing`) | Grid (now) |
|---|---|---|
| frames over 11ms | 360, 355, 347, 337 | 101, 98, 98, 95 |
| 1% low fps | 45.8, 46.3, 44.4, 42.8 | 49.9, 47.4, 47.8, 50.1 |
| average fps | 98 to 100 | 124 to 126 |
| a step settles | 1.79 to 1.81s | 1.46 to 1.50s |
| load | 0.80 to 0.82s | 0.62 to 0.65s |

The 1% low is still ~48 fps: under the 90 fps VR needs, below.

### Stream steps settle 3x slower since b53bd6fac (2026-11-23)

`git bisect` with perf_terrain (release, far 8, seed 666): every commit up to e1f455d2d settles a stream step in
0.49 to 0.52s, b53bd6fac "lods swap without seams or dark flashes" and every commit after in 1.39 to 1.42s. That
commit fixed seams and dark flashes by waiting:

- an unsettled chunk (its lod changing, generating) is open to its neighbours, they mesh again once it settles: a
  second mesh for each neighbour
- a lod swap waits for its neighbours' rebuilds; held meshes build again once relit
- light floods and sunbeams have a time budget a frame (lighting spread over more frames: smoother, slower to settle)

Its 1% low stayed ~48 fps (the same as before it), its average streaming fps fell 180 to 125: the extra rebuilds
are work, not only waiting. To look at: fewer second meshes (a neighbour's rebuild only where the border it meets
changed), the swap's waits.

### What's in the slow frames

From `--profile` runs (release, far 8, cpu meshing):

| System | Its worst frame | What it is |
|---|---|---|
| `region_towns_system` | 16ms, once | a new region's towns placed, one job on one thread |
| `chunk_colors_build_system` | 13 to 15ms, at start | colours built in one go |
| `app_gpu_end_system` | up to 12ms | the frame waiting on the gpu |
| `light_beam_system`, `light_flood_system` | ~3ms each, every streaming frame | sunbeams and floods (budgeted) |
| `build_smooth_lights_system` | 3.8ms (sides meshing), 5.2ms (grid meshing) | a mesh's vertex lights |
| `chunk_sides_system` | 4.7ms (sides meshing only) | SidesOctree built (the grid removes it) |
| between systems | gaps up to 8.8ms | sync points (merges), after `skybox_set_time_system`, before `voxel_octree_optimize_system` |

- The stream moves tags on ~60000 entities in a run (BuildMesh, BuildChunkSides, BuildMeshColors, MeshColorsDirty,
  MeshDirty): each an add and a remove.
- Systems without timing spans cost flecs' time each frame anyway (`GenerateIncrementSystem` 302ms over the run,
  the dirty increment systems 100 to 180ms each): small a frame, every frame.

## What to do, in order

1. **One grid for a mesh and its lights.** With grid meshing the lights fill their own copy of the voxel grid the
   mesher just filled (sys/build_grid.c `chunk_voxel_grid`): the lights' worst frame went 3.8 to 5.2ms. Fill it once:
   the mesh and its lights in one pass, or the grid kept on the mesh until its lights are built.
2. **Time budgets for meshing and smooth lights,** sized for an 11ms frame: `chunk_textured_build_system` and
   `build_smooth_lights_system` stop at a count of meshes, not a time (`max_process`); `chunk_sides_system` and the
   light passes already have budgets (`zox_sys_budget`).
3. **Long one off jobs split over frames:** `region_towns_system`, `chunk_colors_build_system`, the wfc model
   observer at a realm's start (~960ms, before streaming, doc/todo.md).
4. **The 8.8ms gaps:** which merge or sync point (doc/engine/thread_safety.md, `tst pipeline_syncs`), and whether a
   system there can move phase.
5. **Fewer second meshes after b53bd6fac** (above).
6. **perf_terrain fails on frame drops:** a cap on frames over 11ms while streaming, once they're down, so a change
   that brings them back fails.
