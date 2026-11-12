# Systems and Phases

How a frame runs: systems in phases, in order. See [architecture.md](architecture.md) for systems vs events.

## A frame

Zoxel runs its own pipeline of phases ([src/_/flecs/dat/pipelines.c](../src/_/flecs/dat/pipelines.c)), each after
the one before:

| Phase | What runs there |
|---|---|
| `zoxp_dbg_begin` | debug, start of the frame |
| `zoxp_begin` | |
| `zoxp_spawn` | spawning: characters, ui, chunks' npcs, stats (`zoxp_inputs_extract` too) |
| `zoxp_gpu_upload` | meshes and textures to the gpu |
| `zoxp_octree_write` | voxel octrees written (blocks placed, terrain generated) |
| `zoxp_octree_sync` | a barrier (`zox_phase_barrier`): workers finish octree writes before reads, see [thread_safety.md](thread_safety.md) |
| `zoxp_octree_read` | voxel octrees read (chunk sides, lights) |
| `zoxp_pre_physics` | state and update (`zoxp_update`, `zoxp_state`, `zoxp_initialize`, inputs) |
| `zoxp_physics` | physics, movement (`zoxp_post_update`, `zoxp_load`), see [physics.md](physics.md) |
| `zoxp_trails` | things following others (trails), after a barrier (`zoxp_trails_sync`), as are the next four |
| `zoxp_layouts` | ui layouts to local positions |
| `zoxp_transform_hierarchy` | local to world transforms, down the hierarchy |
| `zoxp_billboards` | billboards face the camera |
| `zoxp_transform_end` | world transforms to matrices |
| `zoxp_cameras` | view and projection matrices |
| `zoxp_render` | drawing |
| `zoxp_pre_end` | cleanup: removing states, destroying (`zoxp_reset`, `zoxp_remove`, `zoxp_destroy`) |
| `zoxp_end` | time, presenting the frame (`zoxp_time`, `zoxp_render_end`) |
| `zoxp_post_end` | |
| `zoxp_dbg_end` | debug, end of the frame |

The aliases (`zoxp_update` is `zoxp_pre_physics` and so on) are names for what a system does, use them.

- A state tag added in one phase (`Dirty`, `Initialize`) is seen by later phases that frame, removed in
  `zoxp_pre_end` (`zoxp_remove`), so it lasts one frame.
- `PreInitialize` became `Initialize` a frame later so new entities start on the next frame (from before the
  custom pipeline).

## Kinds of systems

Registered in a module's `sys/_.c` with a query (see [coding_standards.md](coding_standards.md#system-files)):

| Macro | Runs | Use for |
|---|---|---|
| `zox_system` | on worker threads, deferred | most work |
| `zox_system_1` | on the main thread, deferred | anything that spawns or deletes entities (flecs can't create entities from worker threads) |
| `zox_system_immediate` | main thread, not deferred | bulk spawning (`zox_bulk_spawn`), rarely |

- **Deferred**: changes (adds, sets, spawns) are queued, then merged after the system's phase. A component set
  this frame isn't seen by `zox_has` / `zox_get_link` on a just spawned entity until the merge.
- **Immediate** systems make flecs merge everything before them (a sync point), it changes the order things
  happen in: one in `zoxp_spawn` stopped the pause menu opening. Use `zox_sys_world2()` in them (the world, not a
  stage). Prefer not to.
- Multi threaded systems' times add up every worker's (the profiler divides by them).
- Workers only wait for each other at merges, not between phases: a main thread system makes merges either side
  of it, see [thread_safety.md](thread_safety.md) and `tst pipeline_syncs` (logs the pipeline and its merges).

## Doing less a frame

- `zox_sys_budget(name, ms)`: a time budget per worker a frame, the rest waits for the next frame, at least one
  item always runs (`chunk_sides_system`). Better than counting processed items.
- A spawner's cost is mostly after it (the merge, the observers), so a time budget there doesn't measure it,
  see [spawning.md](spawning.md).

## Finding slow frames

- `--profile`: every frame over 25ms is logged with its systems, merges, observers, commands, tables made, the
  components moved most and the prefabs spawned most.
- `bash bsh/tst.sh perf_stream`: crossing chunks, the slowest frames and systems, `tables_stream`: the tables made.
