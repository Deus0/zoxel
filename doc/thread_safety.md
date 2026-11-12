# Thread Safety

How systems share data across worker threads, where flecs makes them wait, and how octrees are kept safe.
Learned chasing an intermittent segfault in `stream_npcs` (2026-11-12). See [systems.md](systems.md) for phases.

## How flecs runs systems on workers

- A multi threaded system (`zox_system`) is split across workers: each worker gets its share of the entities.
- The pipeline is cut into **ops**, runs of systems between merges. Inside an op each worker runs its share of
  every system in turn, **without waiting for the other workers between systems**. Worker 1 can be in system B
  while worker 2 is still in system A.
- Workers only wait for each other at a **merge** (sync point), where the deferred commands are applied.
- **Phases are not merges.** Two phases next to each other run in one op unless something makes a merge.

Flecs makes a merge (`flecs_pipeline_build` in flecs.c) when:

- execution switches between multi threaded and main thread systems (`zox_system` <-> `zox_system_1`)
- an immediate system runs (`zox_system_immediate`)
- a system reads (from a non `$this` source) a component another system wrote through commands

Writing a `[out]` component of the system's own entities (`$this`) never makes a merge.

## Seeing the merges

`bash bsh/tst.sh pipeline_syncs` logs the pipeline as it runs in game, a line per phase (wrapped), in
bin/tst/pipeline_syncs.log:

```
zoxp_octree_write: combine_vox_system voxel_octree_optimize_system voxel_update_queue_system ... vodes_remove_system |
zoxp_trails_sync: zoxp_trails_sync_write | zoxp_trails_sync_wait
zoxp_octree_read: chunk_colors_build_system vox_lod_mesh_build_system chunk_sides_system ...
```

- `|` is a merge, `[1]` a main thread system, `[!]` an immediate one, nothing is multi threaded.
- It lists main thread systems between multi threaded ones: each costs 2 merges, and fails on any.
- The metric line counts systems and merges, it fails over 12 merges (11 on 2026-11-12, from 19: the barriers
  below, `ProgressTasks` disabled, test probes and the sound listener moved into `zoxp_render`).
- How: `ecs_pipeline_stats_get(world, ecs_get_pipeline(world), &stats)`, `stats.systems` are the active systems
  in order with a 0 at each merge (needs `FLECS_STATS`, defined in src/defines.c).
- Test probe systems (`Tst*ProbeSystem`) run in `zoxp_render`, so tests have the game's pipeline.
- `ProgressTasks` (flecs script's async tasks, immediate, unused) is disabled in `zox_ecs`.

## Keeping merges few

- A merge waits for every worker and applies every queued command, the pipeline's slowest points.
- Keep main thread systems together: spawning in `zoxp_spawn`, gpu work in `zoxp_gpu_upload`, `zoxp_render`.
- **A system that doesn't need the main thread is multi threaded.** Main thread is for spawning and deleting,
  gpu calls, and state shared outside flecs (the mixer, sdl).
- A main thread system alone in a multi threaded phase splits it, 2 merges (listed by `pipeline_syncs`).
  `sound_listener_system` was one, moved to `zoxp_render` with the other main thread systems.
- Add a merge on purpose only as a barrier (below), and say why where it's registered.

### Barriers

`zox_phase_barrier(world, name, after)` (src/_/flecs/dat/pipelines.c) makes a phase where every worker finishes
the phases before it before any start the phases after, with **one merge and no main thread**:

- a writer system declares a staged write of the `ZoxSync` tag (`[out] ZoxSync()`, no source), the waiter after
  it reads `ZoxSync` on an entity that has it (`zox_sync`), flecs merges between them (the rule above)
- both empty and multi threaded: the systems either side stay multi threaded
- a main thread system as a barrier makes 2 merges, and the other cores wait while it runs

Barriers (2026-11-12), the transform chain: physics ->
`zoxp_trails_sync` -> trails -> `zoxp_layouts_sync` -> 3D layouts -> `zoxp_hierarchy_sync` -> hierarchy ->
`zoxp_billboards_sync` -> billboards -> `zoxp_transform_end_sync` -> matrices. Each step reads what the one before
wrote. `trail_system`, `layout3_system`, `transform3_children_system` and `billboard_system` were main thread to be
those syncs (the 2026-09-18 crash fixes), now multi threaded: each root writes only its own subtree (the roots
with parents are chunks, under the app, which has no transform).

- Fewer: the chain is 5 of the 11 merges, billboards could rotate inside the hierarchy walk (1 less).

## Octrees

Octrees (`VoxelNode`) are pointer trees: a write can open (allocate) or close (free) nodes, a reader walking it
at the same time can follow a freed pointer, a segfault.

- **Phases:** writes in `zoxp_octree_write`, reads in `zoxp_octree_read`, with no barrier between them: every
  reader and writer locks the octrees it touches. A lock only makes the cores on the same octree wait, a barrier
  makes them all wait. (`zoxp_octree_sync` was a barrier there until every octree system locked, 2026-11-12.)
- **Locks:** `VoxelNodeLock` (a spinlock per octree), helpers in src/chunks/chunks3/fun/locks.c:
  `zox_octree_lock(world, e)` (NULL if none), `zox_octree_lock_begin/end`, and `zox_lock_voxel_trees` for a chunk
  and its neighbors (address order, no deadlocks). Not reentrant: never lock one you hold.
- **Readers lock** (2026-11-12): chunk colors (and neighbors), chunk sides, vox lod and skeleton meshes
  (`build_vox_mesh_at_depth`, the vox), raycasts (the chunk or vox walked, one per call, nested block vox calls
  hold their own), terrain collisions and inside block, frustum and vodes lod walks, vox textures, smooth and dark
  lights, dungeon expansion's checks.
- **Writers lock:** around their writes, the `write_lock_VoxelNode` markers they had: model generation, model
  nodes (fill, paint), landfill, vegetation, town walls, combine, clone (and its source, address order), vodes
  remove and despawn, chunk load, the voxel update queue, the optimizer, light flood and beam.
- A new octree system locks too, or the octrees race again: a crash, not a stale read.
- **Main thread systems** (`zox_system_1`) run alone in their op, any octree access there is safe.

### Known risks (2026-11-12)

- `chunk3_load_system` (`zoxp_physics`) and `landfill_chunk_system` (`zoxp_octree_write`) both write new chunks,
  in different ops, both locked now. Moving the load into the write phase would race landfill's deferred
  `GenerateChunk` (which runs first), not the octree.
- Spinlocks spin: a long write (a model generating) keeps a reader of that model's octree spinning. Snapshots
  (copy under the lock, work after) would shorten reads if it shows in profiles.

## Rules

- Lock every octree you read or write (`zox_octree_lock`), only for the walk, never across frames or while
  holding another (use `zox_lock_voxel_trees` for several).
- Write octrees in `zoxp_octree_write` where you can, read them in `zoxp_octree_read` or later.
- Never keep octree pointers across frames.
- Spawning, deleting, adding or removing components from workers is deferred (commands), safe. Reading another
  entity's component on a worker is safe if nothing writes it in that op.
- Run `pipeline_syncs` after changing phases or making a system main thread.
