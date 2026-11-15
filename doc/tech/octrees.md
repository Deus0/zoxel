# Octrees

How voxels, lights, sides and bones are stored, what reads and writes them, what they cost, and what's worth
changing. Measured 2026-11-15 (see "Cost").

Code: [src/_/octrees](../../src/_/octrees) (the generic functions, no world needed), the types in
[chunks3/com](../../src/chunks/chunks3/com) and [lights3/com](../../src/lights/lights3/com). Tests:
`bash bsh/tst.sh octree_basics` and the other `octree_*` tests ([src/_/octrees/tst](../../src/_/octrees/tst/_.c)).

## A node

```c
{ void* ptr; byte value; [byte type;] }     // 16 bytes with padding
```

- `ptr` first, `value` right after it: every function works on any node type through its size (`stride`) and its
  `type_offset`, there's one implementation ([base.c](../../src/_/octrees/base.c)).
- Closed: `ptr` is NULL, the node is its `value` for its whole cube. Open: `ptr` is a block of 8 children (one
  malloc of `8 * stride`), in x | y << 1 | z << 2 order (`byte3_octree_array_index`).
- Linked (`type == node_type_instance`, VoxelNode only): `ptr` is an `OctreeLink` to an entity (a block vox, a door),
  not children. Everything that follows `ptr` goes through `octree_children`, so links are never walked into; they
  count as leaves with their value.
- An open node's own `value` is what it had when it opened, unless a pass rewrote it:
  `optimize_octree_node` sets parents to their children's most common value (their lod value, air never wins for
  voxels so thin things stay visible), light trees read the max of the leaves instead (`octree_max_leaf_value`).
  **Never trust an open node's value without knowing which pass ran**: the bone weights read stale ones at lower lods
  (fixed with `bone_parts_most`, 2026-11-15).

## Types

| Type | Kind | Holds | Default |
|---|---|---|---|
| `VoxelNode` | linked | a chunk's or model's block ids | 0 (air) |
| `LightNode` | basic | light levels | `darklight` |
| `SidesOctree` | basic | which faces of a voxel show | 0 |
| `BoneNode` | basic | which body part a voxel came from (doc/done/skeletons.md) | 0 |

`zoxc_octree_basic(T, byte, default)` or `zoxc_octree_linked(...)` ([components.c](../../src/_/octrees/components.c))
makes the component with its hooks (the tree is freed on remove, cloned on copy, moved on move) and its typed
functions: `create_T`, `close_T`, `collapse_T`, `clone_T`, `get_children_T`, `has_children_T`, `getv_T`, setters,
neighbour reads. `zox_octree_functions` alone gives the functions without a component, for tests.

## Reading and writing

- `walk_octree(node, depth, position, ..., mode)` ([root.c](../../src/_/octrees/root.c)): `STOP` returns the deepest
  node there is toward a position (a closed node above the depth answers for it), `OPEN` allocates down to the depth
  (NULL if a link is in the way).
- `get_octree_value` / `getv_T(node, depth, position)`: a walk and a read, up to `depth` pointer hops.
- `set_octree_value2` / the typed setters: open down, set, optionally collapse back up.
- Neighbours: `octree_getv_nearby` reads across a chunk's edge from its neighbour's tree; with `leaf_max` a coarse
  lod reading a detailed tree gets its leaves' max.
- After a batch of writes: `reduce` (8 equal closed children collapse into their parent), `optimize` (parents get lod
  values), or both in one pass (`optimize_reduce_octree_node`).
- `octree_fill_grid` (root.c): a tree's voxels at a depth into a flat grid of bytes, clipped to the grid (a
  neighbour's border slab costs only that slab), `leaf_max` for lights. For systems that read most of a tree.
- [flatten.c](../../src/_/octrees/flatten.c): the tree as one array of uints (value in 8 bits, first child index in
  24), the layout a gpu shader reads (doc/plans/gpu_meshing.md), breadth first.

## Threads

A tree is read and written from worker threads (generation, sides, lights, meshing, the body merge). Each tree
entity has a `VoxelNodeLock` spinlock; a system locks its own and its neighbours' in address order
(`zox_lock_voxel_trees`, chunks3/sys/sides.c) so two chunks never wait on each other. doc/thread_safety.md has the
rules. A neighbour link must be a chunk: one that wasn't left the lock unset and crashed (fixed 2026-11-15, the
cause was flecs reviving a deleted id, doc/flecs_patches.md).

## Cost

Flying at Far 8 on the 780M, an -O3 build with tests, fixed seed, every thread sampled (`ZOX_SAMPLE_ALL`,
doc/commands.md):

- Octree work is about 8% of the cpu: `chunk_sides_system` 8.2% with `build_sides_dig` 3.4% inside it,
  `flood_light` 1.6%. Nothing else from the octrees was in the top 30 functions.
- About 25% is flecs walking queries on 16 workers, the frames over 25ms are the gpu swap and merges
  (doc/optimize.md). So the octrees are not what limits flying today.
- Memory: 16 bytes a node, 128 a block of 8. A full depth 5 chunk (32^3) is 37449 nodes, ~600kb before `reduce`;
  terrain reduces well (uniform stone and air collapse), a noisy surface chunk doesn't.
- A read is `depth` dependent loads (a cache miss each when cold). Systems that read every voxel of a tree
  (meshing a model, the bone weights) flatten to a grid first: the bone weights went from 31-36ms a body to 6.4ms
  that way.

## What's worth doing

Option 1 is under way (the user picked it). Options 2 to 4 change the architecture: the user is unsure about them,
so they're research notes, not plans; each would need its own doc/plans doc and measurements first.

1. **Dig with the tree, not into it** (the user picked this one, 2026-11-15). Where the per voxel reads are, by
   their share of the cpu flying: smooth lights 4.6% (`build_smooth_lights_system`, `smooth_face_lights`,
   `zox_apply_smooth_lights`), the light flood 5% (`light_flood_system`, `flood_light`: a flood fill, 3 walks and
   2-3 locks a step, capped at 256 steps a run), then small ones (vox_texture.c, combine.c, ground.c, the
   collisions). Done: smooth lights read a face's 3 x 3 lights and 4 solids once (13 reads a face, was 21), from a
   grid: a chunk's lights and voxels at the mesh's depth with a voxel of border from its 26 neighbours, flattened
   once a mesh (`octree_fill_grid` in root.c, `smooth_grid_fill`), one lock a tree instead of one a read.
   `build_smooth_lights_system` went from 2.6% to 1.8% of the cpu flying (one sampled run each, with
   `smooth_face_lights` and `zox_apply_smooth_lights` inside it); tst smooth_lights and lod_lights compare the
   grid with the trees for every face. Next: the flood on a grid (it writes too, so the grid has to be written
   back), then the small ones.
   General rule: `build_sides_dig` and the mesh builders already recurse; anything still
   calling `getv_T` per voxel in a loop should recurse or flatten. Find them: grep `getv_` inside triple loops.
2. **Blocks of 8 from a pool.** One malloc per opened node today; a per thread pool of 128 byte blocks would cut
   allocator time in generation and remove fragmentation (the heap grows ~22mb a game, todo "Engine"). Measure
   malloc's share in generation first (`ZOX_SAMPLE_ALL`, look for `malloc` and `_int_malloc` leaves).
3. **8 byte nodes.** A 32 bit child index into a per tree array instead of a pointer halves the memory and makes a
   tree one allocation (free is one call, a copy is a memcpy, the gpu layout for free: flatten.c is nearly this).
   A big change: every `ptr` use goes through `octree_children`, which helps, but links and the locks need care.
4. **Sides without a tree.** `SidesOctree` is a second tree a chunk; check what its shape adds over the voxel
   tree's (not read for this doc), a bitmask per leaf block might replace it.

For the user's larger draw distances the count of chunks matters more than a chunk's tree: see todo.md
"Engine" for the 100k chunk stress test (event tags against a byte state).
