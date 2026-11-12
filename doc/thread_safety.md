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
zoxp_octree_write: CombineVoxSystem voxel_octree_optimize_system VoxelUpdateQueueSystem ... VodesRemoveSystem |
zoxp_octree_sync: zoxp_octree_sync_write | zoxp_octree_sync_wait
zoxp_octree_read: ChunkColorsBuildSystem VoxLodMeshBuildSystem chunk_sides_system ...
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
  `SoundListenerSystem` was one, moved to `zoxp_render` with the other main thread systems.
- Add a merge on purpose only as a barrier (below), and say why where it's registered.

### Barriers

`zox_phase_barrier(world, name, after)` (src/_/flecs/dat/pipelines.c) makes a phase where every worker finishes
the phases before it before any start the phases after, with **one merge and no main thread**:

- a writer system declares a staged write of the `ZoxSync` tag (`[out] ZoxSync()`, no source), the waiter after
  it reads `ZoxSync` on an entity that has it (`zox_sync`), flecs merges between them (the rule above)
- both empty and multi threaded: the systems either side stay multi threaded
- a main thread system as a barrier makes 2 merges, and the other cores wait while it runs

Barriers (2026-11-12): `zoxp_octree_sync` (octree writes before reads), and the transform chain, physics ->
`zoxp_trails_sync` -> trails -> `zoxp_layouts_sync` -> 3D layouts -> `zoxp_hierarchy_sync` -> hierarchy ->
`zoxp_billboards_sync` -> billboards -> `zoxp_transform_end_sync` -> matrices. Each step reads what the one before
wrote. `trail_system`, `Layout3System`, `transform3_children_system` and `billboard_system` were main thread to be
those syncs (the 2026-09-18 crash fixes), now multi threaded: each root writes only its own subtree (the roots
with parents are chunks, under the app, which has no transform).

- Fewer: the chain is 5 of the 11 merges, billboards could rotate inside the hierarchy walk (1 less).

## Octrees

Octrees (`VoxelNode`) are pointer trees: a write can open (allocate) or close (free) nodes, a reader walking it
at the same time can follow a freed pointer, a segfault.

- **Phases:** writes in `zoxp_octree_write`, reads in `zoxp_octree_read`.
- **Barrier:** `zoxp_octree_sync` between them (a barrier, above), every worker finishes writing before any
  reads (`pipeline_syncs` checks it). Before it, write and read systems shared
  an op: `voxel_octree_optimize_system` freeing nodes while a mesh builder on another worker walked them.
- **Locks:** `VoxelNodeLock` (a spinlock per octree) guards writes and reads that cross chunks in one phase:
  `VoxelUpdateQueueSystem`, `voxel_octree_optimize_system`, light flood and beam, chunk sides (locks its
  neighbors in address order, no deadlocks).
- **Unlocked readers** rely on the phases: chunk colors, vox lod and skeleton meshes (they read another entity's
  octree, the vox or skeleton owner), raycasts, collisions, vox textures.
- **Main thread systems** (`zox_system_1`) run alone in their op, any octree access there is safe.

### Known risks (2026-11-12)

- `chunk3_load_system` writes octrees in `zoxp_physics` (`zoxp_load`), the same op as `terrain_collision_system`,
  `Chunk3RaycastSystem` and `InsideBlockSystem` reading them. Only with saves (tests run `--nosaves`).
  Moving it to `zoxp_octree_write` races `landfill_chunk_system` on new chunks (load sets `GenerateChunk`
  deferred), it needs its own phase between barriers or to run on the main thread.
- `zoxp_octree_read` runs on into `zoxp_pre_physics` in one op: model node systems (`FillModelNodeSystem`,
  `PaintModelNodeSystem`) write vox octrees while the mesh builders read others. Different entities, a mesh
  builds after its model is generated.
- `VegetationChunk3System`, `CombineVoxSystem`, `landfill_chunk_system` write without locking, they only write
  their own (new) chunk.

## Rules

- Write an octree only in `zoxp_octree_write` (or a main thread system), read it in `zoxp_octree_read` or later.
- Reading a neighbor's octree in the write phase: lock it (`VoxelNodeLock`).
- Never keep octree pointers across frames.
- Spawning, deleting, adding or removing components from workers is deferred (commands), safe. Reading another
  entity's component on a worker is safe if nothing writes it in that op.
- Run `pipeline_syncs` after changing phases or making a system main thread.
