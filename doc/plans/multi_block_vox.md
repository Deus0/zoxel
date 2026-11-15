# Multi block vox models (plan)

A world model taller or wider than one block: street lamps (2-3 blocks), tall grass, doors, later trees as
models. The user's worry: it can go across chunks. Short answer: the model doesn't have to, only its blocks do,
and blocks already cross chunks (trees). Not started; open questions at the end.

## How a block model works now

- A chunk's voxel holds a block id. Blocks with a `BlockPrefabLink` are vodes (world blocks: grass, flowers,
  rubble): `spawn_vodes_dive` (vodes3/sys/spawn.c) walks the chunk's `VoxelNode`, and for each vode voxel spawns
  one entity (`spawned_block_vox`), links the voxel's node to it (`link_node_VoxelNode`, a linked node, see
  [tech/octrees.md](../tech/octrees.md)) and parents it to the chunk (non fragmenting).
- The entity is an instanced renderer (`spawn_block_vox_instanced`): a transform at the voxel and an
  `InstanceLink` to one of the block's model variants (`get_model_variant`), drawn by
  `vox_instance_render_system` with every other instance of that mesh. Or a `BlockVox` with its own voxels.
- It's lit by one `Brightness` from the light at its voxel, hidden with its chunk, deleted with its chunk or when
  its voxel changes (vodes3/sys/despawn.c, removed.c).

Nothing in drawing ties the model to its block's cube: the instance is a transform and a mesh.

## The plan: one entity at the base, filler blocks above

1. **A tall model.** The model is one vox, N blocks tall (a lamp: 1 x 3 x 1 blocks of model voxels), its origin at
   the base block's bottom centre. Model octrees are cubes, so a 1 x 3 x 1 model sits in a cube 4 blocks a side,
   mostly air (air collapses, it costs little). Needs: the model's size in blocks on the block (`BlockSize`,
   byte3, 1x1x1 when absent), and the instance transform placed from the base, not centred on a cube.
2. **The base voxel** is the vode: it spawns the one entity, as now.
3. **Filler voxels** above (and beside, for wide models) hold a filler block: solid for collisions and light or
   not as the model wants, no mesh faces, no entity. They make the model block movement, stop other blocks being
   placed inside it, and let a hit on any part find the base.
4. **Across chunks.** Only fillers cross a chunk edge, and they're plain block ids: generation writes them the way
   it writes trees (each chunk draws the voxels of what overlaps it: `regions/fun/place.c`, tst vegetation_chunks),
   placing one by hand writes each voxel through the chunk it falls in (`terrain_voxel_at` finds the chunk). The
   entity stays in the base's chunk.
5. **Breaking it.** Hitting a filler walks to the base (down for tall models: at most N - 1 voxels, across a chunk
   edge through the neighbour), the base's removal clears its fillers, the drop comes from the base.
6. **Placing it.** Every voxel it covers must be air (and loaded), else the placement is refused.

## What it costs

- No change to chunk meshing (fillers have no faces: a block flag the mesh builder and sides skip, like air for
  faces but solid for collisions) or to streaming.
- Hidden with the base's chunk: a lamp whose base chunk is culled or unloaded vanishes whole, its top can pop at
  the edge of view by a block or two. Fine for 2-3 blocks, not for a big tree (that's the "structures across
  chunks" item in todo.md, a different problem: many voxels, drawn by chunks).
- One light value for the whole model (the base's). A lamp is a light source anyway: the lamp head's voxel can be
  a light emitting filler (glow stone's rule), which lights the street.

## Steps (each with a test)

1. `BlockSize` and the base-anchored transform; a 1 x 2 x 1 test block placed by a test, a screenshot shows it two
   blocks tall (tst model_views style).
2. Filler block: collides, no faces, no entity; a character can't walk through the top, the mesh has no faces
   for it (tst like terrain_collisions).
3. Break and place rules, with the filler in the chunk above (a base at the top row of a chunk).
4. The lamp: model (B's lane: the vox art, round ended), a light emitting head, towns place them along roads
   (towns already place homes and roads into tunks).
5. Fireflies (particles, B's lane).

## Answered by the user (2026-11-15)

- **Faces:** there's no translucent or "solid without faces" block material yet. The plan for materials is a
  refactor of chunk meshes by material: vox -> a chunk material with its sides -> mesh lods (water and others,
  todo.md "Terrain and Chunks: Water Blocks / Chunk Materials"). So the filler is the first block that collides
  without drawing: either a small flag the sides skip now (step 2 below stays small), or it waits for chunk
  materials and becomes "a material with no mesh". Decide when starting step 2; the flag is cheaper and the
  materials refactor can absorb it.
- **Scale:** `vox_model_scale` (1/64) was only set so block voxes matched the original blocks. Nothing depends on
  it; a model's scale should be flexible, so a tall model sets its own size in blocks (`BlockSize`).
- **Filler id:** one shared block id. Blocks are bytes, ids are scarce.
- **Doors:** need an open and closed state, not worked out yet; possibly per block data stored for the custom
  blocks that need it. That data (a state a voxel, saved with the chunk) is its own design, with doors.

## Still open

- Finding the base from a shared filler id: straight down for tall models; wide models need a small search or
  the offset kept in per block data (the same store doors would use).
- Rotated models (a door on any wall): with doors.

## Build order: bed and door first (the user, 2026-11-15)

A bed (2 wide, rotatable) and a door (2 tall), both generated models, both turned by the way the placer faces. The
lamp comes after, on the same support. What the code looks like today, read for this:

- A block is 32 model voxels a side at depth 5 (`vox_model_scale` 1/64, a block 0.5 units). A model at depth 6 is
  two blocks a side: the bed fills 64 x 32 x 32 of it, the door 32 x 64 x 32, the rest air.
- Generated models: a `VoxType` and a system that writes the `VoxelNode` (models/generation/flowers.c with
  models/gen/flowers.c is the pattern), spawned with `spawn_model_lods_generated`, made a block with
  `spawn_realm_block_model` (terrain/ins/blocks_decor.c), an item a block by `items_realm_spawn_system`.
- Placing: `item_activate_system` (items/sys/activate.c) adds a `VoxelNodeUpdate { position, value }` to the hit
  chunk's `VoxelNodeQueue`; `voxel_update_queue_system` writes it. The vode then spawns from the voxel
  (`spawn_vodes_dive`), at the voxel's centre, unrotated (`spawn_block_vox_instanced`).
- Collisions read `BlockCollider` by block id, faces read `BlockModel`: a filler is a block with a solid collider
  (or air, for openings), a non cube model and no prefab. Two shared ids: `filler` and `filler open`.

Steps, each with its test:

1. **Models.** `vox_type_bed` (a frame, a mattress, a pillow at one end, a blanket) and `vox_type_door` (planks, a
   frame, a handle), seeded colours like the other generated blocks. tst: a model_views style screenshot, and the
   voxels' bounds are 2 x 1 x 1 and 1 x 2 x 1 blocks.
2. **Size and anchor.** `BlockSize` (byte3, blocks) on the block; the vode's transform placed from the base voxel
   so the model covers base + size, not centred on one cube. tst: a bed and a door placed by a test, their mesh
   bounds in the world.
3. **Fillers.** The two filler blocks on the realm; placing a sized block also queues its fillers (through
   `terrain_voxel_at` for a voxel in the next chunk), refused if any covered voxel isn't air; removing any part
   removes all, the base drops the item. tst: collide with the bed's second block and the door's top, break a
   filler, place across a chunk edge.
4. **Rotation.** The placer's facing picks one of four directions; the model turns about the base, its fillers go
   that way. Needs the direction stored for the placed block (below). tst: placed facing each way, the fillers and
   the mesh bounds follow.
5. **Door state** (open, closed), after the above: the same store.

### Where a placed block's direction lives (needs the user's nod before step 4)

A block id is a byte, so the direction can't be in it without an id a direction. The proposal, the "per block data
for custom ones" the user mentioned:

- `BlockStates` on a chunk: a short list of `{ byte3 position, byte state }`, only for voxels that have a state
  (beds, doors: a few a chunk at most). The low 2 bits are the direction, the next bit a door's open state; other
  custom blocks get the rest.
- Written with the voxel: `VoxelNodeUpdate` gains a `state` byte, `voxel_update_queue_system` adds or removes the
  entry under the chunk's voxel lock. Read when the vode spawns (its rotation) and by whatever uses the block.
- Saved with the chunk: chunks3/io/sys/save.c and load.c gain a block states section. **This changes the chunk
  file format**: old saves must still load (no section = no states), and the format needs a version if it has
  none (not read yet).
- A filler finds its base by searching the voxels round it, at most the largest size away, for a base whose box
  (from its size and direction) holds it.

Alternatives: four block ids a rotatable block (8 ids for a bed and a door, of 255: the user wants ids kept); or
the direction on the vode entity only (lost when the chunk unloads, so it would still have to be saved somewhere).

**The user's answer (2026-11-15): yes, on two conditions.** It must cost little on disk and stay efficient for
large chunks, and it needs an io test. So: nothing is written for a chunk with no states (most chunks); a chunk
with states writes a count and 4 bytes a state (3 for the position, 1 for the state); the io test saves a chunk
with a bed and a door facing different ways, loads it, and checks the states and the file's size (a chunk with no
states is byte for byte what it was before).

