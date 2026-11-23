# Blocks

How a placed block's data is stored: its id, its own data, its world block entity. What reads and writes each, and
what's planned. Written 2026-11-19 with doors (src/vodes/doors). It's the state for now, to be consolidated (see
"Planned").

## Three places a placed block lives

| | what | per | saved | code |
|---|---|---|---|---|
| `VoxelNode` | the block id (a byte, 1 + its index in the realm's `BlockLinks`, 0 air) | voxel, in an octree a chunk | yes, the chunk's octree | [octrees.md](octrees.md) |
| `BlockDatas` | bytes for the few voxels that need them | chunk, one packed array | yes, a section after the octree | [chunks3/fun/block_data.c](../../src/chunks/chunks3/fun/block_data.c) |
| world block entity | a block vox model's entity (grass, a bed, a door), linked from its voxel's node | voxel with a model | **no** | [vodes/vodes3](../../src/vodes/vodes3) |

- A block kind (stone, a door) is an entity of the realm, `BlockLinks` holds them; what all blocks of a kind share
  is on it (`BlockCollider`, `BlockSize`, `BlockDoor`), not per voxel.
- World block entities are spawned by `vodes_spawn_system` (while a game plays) and destroyed by
  `vodes_despawn_system` once their chunk drops below full detail. There's no io for them: anything only on them is
  lost when you walk a chunk or two away, or reload. That's why a door's open state isn't only on its entity.

## BlockDatas

One buffer a chunk, entries packed one after another, no pointers (moves and copies are a memcpy):

```text
position x, y, z (3 bytes) | length (1 byte) | data (length bytes, 1 to 255)
```

- Empty (no memory) for most chunks; a chunk with none writes nothing to its file.
- Saved after the chunk's octree as a section: an id byte (`block_datas_section` 1), a 2 byte size, the buffer.
- A lookup (`find_block_data`) walks the entries one by one: fine for a few a chunk, slow for many.

What the one byte holds today (`get_block_data` / `set_block_data`):

| voxel | byte |
|---|---|
| a bed's or door's base | 1 + its direction (quarter turns about up, 0 to 3): 1 to 4 |
| a filler (the rest of a bed or door) | 1 + its offset back to its base (`block_offset_encode`, 2 blocks each way): 1 to 125 |

`block_data_direction` and `block_offset_decode` mask out bit 128.

### Who writes it

- `voxel_update_queue_system` (zoxp_octree_write), with the voxel: a `VoxelNodeUpdate`'s `data`, set when its voxel
  changes, cleared when it's removed or replaced. Placing a bed or door goes through here (`multi_block_place`).
- `block_toggle` (items/fun/multi_block.c, used by `door_toggle` and the gravity cube; `terrain_set_block_data`): straight into the chunk's buffer under its octree
  lock, the chunk marked `Edited`; the voxel stays. Not through the queue: the queue only writes data when its voxel
  changes, and `drops.c` treats any queued update over a block as a break. It runs on a worker
  (`block_activate_system`, vodes/activations): writes under the chunk's lock and deferred sets are safe there;
  spawning isn't (flecs asserts), so its sound is a one-off event (`DoorSound`, a non fragmenting tag, `on_door_sound`).

### Who reads it

- `vodes_spawn_system`: a sized model's direction (its turn and where it sits).
- the door hook (`door_on_spawned`): a door's world block entity spawns open or closed.
- `chunk3_raycast`: a filler's offset to its base, the ray goes into the base's model.
- `multi_block_removed`: a removed part's fillers or base.
- terrain collisions (`collision_block_solid`, intersect2.c): a block whose `BlockCollider` is `zox_block_open` (a
  door, its filler) on in its chunk's `ActiveOctree` is passed through; other blocks switched on stay solid. Only
  looked up in chunks with something on; read without a lock, like the octree there (collisions and octree writes
  run in different phases).

## Using a block (activations)

`block_activate_system` (src/vodes/activations, multithreaded): an entity with `TriggerActionB`, `RaycastVoxelData`
and `RaycastRange`, the frame its right click is active, its ray on a block in reach: the block's kind picks what
happens (`block_use`), a door `door_toggle`, a bed `character_sleep` (vodes/beds), a gravity cube `block_toggle`
(its field round it again, terrain/gravity, doc/tech/physics.md). tst block_activate, gravity_cube.

## Doors now

- Open state: on in its chunk's `ActiveOctree` at every part, so it's saved and collisions see it without the entity.
- The swing: on the world block entity (`DoorOpen`, `DoorSwing`, `DoorRestPosition`, `DoorRestRotation`,
  `DoorHinge`), set from the block data when it spawns, eased by `door_swing_system`.
- So the state is in two places (the `ActiveOctree`, the entity's `DoorOpen`), kept in step by `door_toggle`.

## What's switched on: ActiveOctree (2026-11-23)

A terrain chunk's `ActiveOctree` (chunks3/com, fun/active.c): a byte a voxel, 0 off, on otherwise (a door open, a
gravity cube on; later a manawire's power level). `BlockDatas` keeps what doesn't change (directions, filler offsets).

- Nothing on: its root, one node at 0, so "is anything on here" is its root (`active_octree_any`).
- Set with its voxel (`VoxelNodeUpdate.active`, a door placed open), cleared when its voxel changes (`old_active`
  remembers it: a gravity cube dug out while on); switched straight in the chunk under its lock by `block_toggle`
  (`terrain_set_active`, the chunk `SaveDirty`).
- Read by collisions (doors walked through), lights (`light_stops`), the door hook (spawns open), gravity (the
  cubes on, terrain/gravity).
- Saved as section 2 after the block data, only when something's on (doc/tech/saves.md).
- Planned (doc/todo.md): a `PowerOctree` beside it, worked out and not saved (as light), power spread from generator
  blocks through manawires; a parent's value the max of its children for coarse lod checks.
