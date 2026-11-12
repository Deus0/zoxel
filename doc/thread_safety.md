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
zoxp_octree_sync: [1]octree_sync_system |
zoxp_octree_read: ChunkColorsBuildSystem VoxLodMeshBuildSystem chunk_sides_system ...
```

- `|` is a merge, `[1]` a main thread system, `[!]` an immediate one, nothing is multi threaded.
- It lists main thread systems between multi threaded ones: each costs 2 merges.
- The metric line counts systems and merges (407 systems, 21 merges, 2026-11-12).
- How: `ecs_pipeline_stats_get(world, ecs_get_pipeline(world), &stats)`, `stats.systems` are the active systems
  in order with a 0 at each merge (needs `FLECS_STATS`, defined in src/defines.c).
- Tests add their own main thread probe systems (`Tst*ProbeSystem`), which add merges the game doesn't have.

## Keeping merges few

- A merge waits for every worker and applies every queued command, the pipeline's slowest points.
- Keep main thread systems together: spawning in `zoxp_spawn`, gpu work in `zoxp_gpu_upload`, `zoxp_render`.
- A main thread system alone in a multi threaded phase splits it, 2 merges (listed by `pipeline_syncs`):
  `trail_system`, `Layout3System`, `transform3_children_system`, `billboard_system` (2026-11-12), candidates to
  be multi threaded or moved. `SoundListenerSystem` was one, moved to `zoxp_render` with the other main thread
  systems.
- Add a merge on purpose only as a barrier (below), and say why where it's registered.

## Octrees

Octrees (`VoxelNode`) are pointer trees: a write can open (allocate) or close (free) nodes, a reader walking it
at the same time can follow a freed pointer, a segfault.

- **Phases:** writes in `zoxp_octree_write`, reads in `zoxp_octree_read`.
- **Barrier:** `zoxp_octree_sync` between them holds `octree_sync_system`, an empty main thread system, so every
  worker finishes writing before any reads (`pipeline_syncs` checks it). Before it, write and read systems shared
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
