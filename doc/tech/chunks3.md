# Chunks3: Voxel Meshes

How voxel octrees (terrain chunks, vox models, characters) become meshes, what's supported, and the plan for greedy
meshing. Code in
[src/chunks/chunks3](../../src/chunks/chunks3), lighting in [src/lights/lights3](../../src/lights/lights3).

## What's supported (2026-11-25)

| | Textured (terrain, blocks) | Colored (vox models) |
|---|---|---|
| A single chunk | `chunk3_single`: a textured chunk on its own (`prefab_chunk_textured`) | `chunk3_single_colored`: a vox (`prefab_vox`), a model with lods (`prefab_vox_lods`) |
| Many chunks under a parent | `spawn_chunk3_parent_textured` (terrain is one, streamed) | `spawn_chunk3_parent_colored` (models bigger than a chunk: bosses) |
| What a voxel is | a block: the parent links its blocks (`BlockManagerLink`) and `Tilemap` | a palette entry: `ColorRGBs` on the model or parent |
| Materials | a block's `BlockMaterial` (solid, liquid, oil) | `MaterialIDs` a palette entry (as MagicaVoxel's), only on a model with more than solid |
| See through | water and oil: a mesh of their own, drawn after, blended (`TransparentMesh`) | a see through mesh beside the solid one (`TransparentMesh`), its `MaterialLink` to a `TransparentMaterial` with `Alpha` |
| Faces between chunks | hidden by the neighbours' voxels, any lods side by side | hidden by the neighbours' voxels |
| Lods | a mesh a lod a material, swapped as a chunk's lod changes (`sys/transition.c`) | a mesh a lod, children of the model, found by `RenderDepth` (`get_model_lod_mesh`) |
| Per vertex | tilemap uvs, smooth lights (terrain's ambient occlusion) | palette colour darkened by ambient occlusion, times a brightness from the world's light (block voxes) |
| Drawn | `chunk3_textured_render_system`, water `chunk3_transparent_render_system` | instanced (`vox_instance_render_system`) or as they are (`vox_render_system`); see through after, blended, no depth writes |
| CPU meshing | the voxel grid (`sys/build_grid.c`) | one walk (`build_voxel_mesh`, `build_voxel_mesh_material`) |
| GPU meshing | solid materials (`lights3/sys/gpu_mesh.c`), textured, lit, with neighbours | not yet (doc/plans/chunks_refactor.md phase 4) |

Both write faces through one writer (`mesh_add_face`, [fun/faces.c](../../src/chunks/chunks3/fun/faces.c)) and use one
hiding rule (`block_material_hides`): a solid voxel hides any face against it, a see through one only its own kind's.
So a solid core shows through a see through shell, and the shell draws only its faces against air.

Tests: `chunk_sides` (borders and lods), `chunk_uvs` (every block's uvs), `block_sides` (a voxel's showing faces),
`vox_materials` (a core in a shell at the mesher), `vox_see_through` (a model's two meshes), `slime_view` (drawn, a
screenshot), `colored_parent` and `colored_parent_shell` (models across chunks), `perf_meshing` (speed and face counts),
`stream_seams` and `chunk_mesh_flicker` (terrain as it streams).

## Chunk3 parents

A chunk3 parent is a vox whose children are chunks, found by position (`ChunkLinks`), meshed together
([pre/chunk3_parent.c](../../src/chunks/chunks3/pre/chunk3_parent.c), [ins/chunk3_parent.c](../../src/chunks/chunks3/ins/chunk3_parent.c)).
What its voxels are lives on it, its chunks take it from it.

- Textured: `spawn_chunk3_parent_textured(world, prefab, realm, blocks, tilemap, depth, scale)`, then
  `spawn_chunk3_parent_chunk(world, prefab, parent, position, render_depth)` for each chunk. It links its blocks
  (`BlockManagerLink`, a realm's `BlockLinks`) and their tiles (`Tilemap`). Chunks with lights are `prefab_chunk_lit`
  (lights3/fun/prefab.c).
  - Chunks side by side at different lods are its job: the faces between them, the light across them (lights3), the
    mesh swapped as a chunk's lod changes (sys/transition.c).
  - A chunk's voxels done (`GenerateChunk` at `zox_generate_terrain_sunlight`): it's lit, then meshed
    (lights3/sys/generated.c). `SunnyChunk` on the top layer takes the sky's light, `BottomChunk` on the bottom ends
    beams. A chunk's voxels or a neighbour changed: its meshes rebuild (`chunk_meshes_rebuild`, `BuildMesh`).
  - Terrain is one that streams its chunks round its streamers, generates them and picks their lods (`prefab_terrain`,
    `prefab_chunk_terrain`).
  - tst fixture `zox_tst_chunk3_parent_ready` (lights/tst/chunk3_parent.c): a realm and 3 x 3 x 3 lit chunks.
- Colored: `spawn_chunk3_parent_colored(world, prefab, palette, depth, scale)` holds the palette (`ColorRGBs`), then
  `spawn_chunk3_parent_colored_chunk(world, prefab_vox, parent, position)` for each chunk: no palette of its own (it
  draws its parent's), linked both ways to its neighbours as it spawns (its first mesh hides the faces between them),
  placed by its centre. `set_chunk3_parent_colored_materials(world, parent, ids, count, material)` adds `MaterialIDs`
  and a see through child to each chunk, filled with its see through voxels as the chunk builds.

## Models with see through parts

A model (`prefab_vox_lods`) is solid unless it has `MaterialIDs`: a material id a palette entry, as MagicaVoxel's,
`zox_block_material_solid` or `zox_block_material_liquid` (see through). `set_vox_lods_materials(world, model, ids,
count, material)` sets them and spawns a see through mesh beside each lod (`TransparentMesh`, the same `RenderDepth`),
its `MaterialLink` to the material: `spawn_vox_material_see_through(world, alpha)`, a `TransparentMaterial` with
`Alpha`. Each lod's solid mesh then holds only its solid voxels, the see through one the rest (its faces against air).
Instances draw the solid mesh in the solid pass and its see through sibling after every solid one, blended at the
material's alpha, testing depth without writing it. Models without see through parts get no second meshes.

## How meshes are built now

Every mesh is one quad (4 vertices, 6 indices) per visible voxel face. Nothing is merged: a flat 16 x 16 floor is
256 quads on its top alone. Vertices aren't shared between quads.

### Vox models (colored meshes)

[sys/build_colored.c](../../src/chunks/chunks3/sys/build_colored.c), entry [sys/vox_lods.c](../../src/chunks/chunks3/sys/vox_lods.c)
(`build_vox_mesh_at_depth`): minivoxes, grass, items, characters' bodies (skeleton meshes), each a mesh a lod (a kind).

- One walk (`build_voxel_mesh_dig`) reading a voxel's neighbours through one lookup: the pyramid (the octree flattened
  into a padded grid of node pointers a depth, an array read, models up to `zox_vox_grid_max_depth` 7) or the octree
  (deeper ones); a neighbour chunk's tree past the border either way. tst `vox_mesh_grid` checks both lookups make the
  same mesh, face for face, in order. A model with no neighbour chunks and no materials takes the pyramid's own face
  step (the fast path).
- A voxel's face shows if its neighbour that way doesn't hide it (`is_adjacent_all_solid`, the hiding rule above).
- Colour is baked a vertex: the voxel's palette colour darkened by **ambient occlusion**:
  - `zox_ambient_occlusion27` (on): each face corner looks at 3 of the voxel's 27 neighbours, its two side
    neighbours and the corner between them ([fun/ao27.c](../../src/chunks/chunks3/fun/ao27.c), `neighbor_indices_27`,
    `compute_ao_factor_27`: both sides solid is full corner darkness, else darker the more are solid); see through
    voxels don't darken corners
  - else [fun/ao6.c](../../src/chunks/chunks3/fun/ao6.c): the 6 face neighbours only
  - so one voxel's face can have 4 different corner colours; that's the shading in creases
- Skeleton meshes: each vertex gets bones by the voxel behind its face (`generate_weights_parts`,
  [bones/sys/generate.c](../../src/bones/sys/generate.c)), it reads meshes as **4 vertices a face** in order, and
  blends a corner between joined bones (doc/done/skeletons.md).

### Terrain chunks (textured meshes)

[sys/build_textured.c](../../src/chunks/chunks3/sys/build_textured.c) (`chunk_textured_build_system`), from
[sys/build_grid.c](../../src/chunks/chunks3/sys/build_grid.c), a mesh per chunk material (solid, water: [fun/materials.c](../../src/chunks/chunks3/fun/materials.c)).

- Which faces show comes from a voxel grid, filled from the octrees as a mesh builds (not kept: the octrees are what's
  real): the chunk's voxels at the mesh's depth as bytes, with a voxel of border from its 6 neighbours, each at the
  depth it draws (`chunk_voxel_grid`, `voxel_grid_fill`), and the gpu mesher's rule (`voxel_grid_shows`): a face
  beside a voxel that doesn't hide it, into a missing chunk only its top. Borders: a neighbour drawn coarser gives its
  big voxel; drawn finer, a cell is solid only if all its voxels along the face hide ours; unsettled (its lod
  changing, generating), air till it settles; not loaded, only top faces. Across chunk borders and lods (tst
  `chunk_sides`); the raycast gizmo asks the same rule from the octrees directly (`get_chunk_sides`, tst
  `block_sides`). It replaced `SidesOctree`, a second tree a chunk built first (2026-11-25, 2.5x a world's meshing,
  doc/plans/chunks_refactor.md).
- UVs: each face gets its block's tile in the tilemap atlas (`tilemap_uvs`, 4 corners a face), liquids their own
  whole texture.
- Light is baked a vertex **after** the mesh, by `build_smooth_lights_system`
  ([lights3/sys/build_smooth.c](../../src/lights/lights3/sys/build_smooth.c)): a vertex averages the light of the
  face's air voxel, its two edges' and its corner's, a corner behind two solid edges counts as dark. That is the
  terrain's ambient occlusion. It walks the same voxel grid **in the same order as the mesher** and writes 4 colours
  a face, so its colours line up with the mesh's vertices by position in the arrays. The upload waits for it
  (`BuildMeshColors`), else a new mesh shows a frame white.

### GPU meshing

[fun/gpu_mesh.c](../../src/chunks/chunks3/fun/gpu_mesh.c), [lights3/sys/gpu_mesh.c](../../src/lights/lights3/sys/gpu_mesh.c),
doc/plans/gpu_meshing.md: a compute shader meshes terrain's solid materials from the same kind of grid, by the same
rule, textured and smooth lit, with its neighbours and their lods (`zox_chunks3_gpu_meshing`: on with vulkan,
`--gpu_meshing`, `--cpu_meshing`). Models aren't gpu meshed yet (doc/plans/chunks_refactor.md phase 4).

### What it costs

- A tall grass block at full detail was 18,032 triangles (tst `vox_instances_stress --gpu --release` logs it); every
  stalk voxel has its own shade, so even a merging mesher couldn't join them.
- Drawn thousands of times (instanced grass), the triangles are the frame's cost now, not the cpu: on a 780M, 16000
  grass instances all in view take 117ms a frame with the render system under 1ms.

## Greedy meshing (planned)

Merge neighbouring faces that look identical into one bigger quad. A flat floor of one block becomes one quad a
slice instead of one a voxel. It has to keep the look exactly: **AO, light, colour, texture and (skeletons) bones
must render the same as now**. That decides what may merge.

### The rule: merge only what interpolates the same

A merged quad has 4 corners; the gpu interpolates between them. So faces may merge only if every corner value is
the same across all of them, so the big quad's corners carry the same values as each small face had at every corner:

- same direction (facing), same slice (depth along the normal)
- same colour (palette index) or, terrain, the same block (tile) and material
- **the same 4 AO / light values on each face, and all 4 equal to each other** (a flat face: no crease darkening on
  it). A face with uneven corners (it sits in a crease, next to a wall, under an overhang) stays its own quad. That
  is the "smaller quads around corners" of doc/todo.md (Refactors): AO is exactly where faces differ, so open floors and
  walls merge, creases keep their detail.
- skeleton meshes: the same bone and no joint blend on it (faces at a joint keep their own quads)
- the same lod depth (an octree node bigger than a voxel is already one quad, merge in units of the meshed depth)

Merging only uniform faces keeps the output pixel identical to today's, which the tests can check.

### The process (a direction and slice at a time, the usual greedy mesh)

1. For each of the 6 directions, and each slice along it (a layer of voxels one deep):
2. Fill a 2D mask, `length x length`, one cell a voxel in the slice: 0 where no face shows, else a **key** of
   everything that must match (colour or block, the 4 corner AO/light values, bone). Compute the AO/light when the
   cell is filled, as the mesher already does a face (ao27 / smooth light's lookups), not afterwards.
3. Walk the mask row by row. At an unused cell with a key:
   - grow along the row while the next cell has the same key (width)
   - grow down rows while the whole width of the next row has the same key (height)
   - emit one quad over width x height, its 4 corners' values the key's
   - clear those cells
4. Next slice, next direction.

Where it fits: the face test and the neighbour lookups stay as they are (the grid's pyramid reads, the sides octree
for terrain). Only the emitting changes: from "add a quad now" to "write a mask cell", plus the mask walk at the end
of each slice. So build it as a pass over the slices after the walkers, not inside the recursive octree digs.

### Making it work with AO and lighting

- **Colored (vox) meshes**: AO is computed from the voxel neighbours at mesh time (ao27), so it can go straight into
  the key. Easy case, do it first.
- **Terrain**: the smooth light is baked later by `build_smooth_lights_system`, matching vertices by order. With
  merged quads that matching breaks (fewer vertices, other positions). Either:
  - bake the 4 corner lights into the mask key in the mesher (it needs the light octrees and their locks then,
    `smooth_face_lights` already computes a face's 4 corners), and keep a "lights changed, re-mesh" path instead of a
    recolour; or
  - keep two passes but make the light pass recompute per quad corner from its position (not by order).
  Light changes far more often than voxels (day, torches, edits) so measure: a re-mesh on every light change may cost
  more than it saves. The first option is simpler; decide with numbers (perf_fly, dig_light, smooth_lights).
- **Textures** (terrain): a merged quad spans several blocks of one tile. The atlas tile can't repeat by itself.
  UVs in voxel units (0..width, 0..height) and the shader repeating inside the tile (`fract` and the tile's offset
  passed per vertex), or a texture array (a layer a block, GL_REPEAT). Mind mipmaps bleeding between atlas tiles.
- **T-junctions**: a big quad next to small ones meets their corners in the middle of its edge, a tiny crack can
  sparkle there. Usually invisible at this scale; if it shows, split the big quad's edge or keep the quads aligned
  to a grid (merge within 4x4 or 8x8 blocks).

### Everything that assumes "a quad a voxel face" (check each)

- `generate_weights_parts` (bones): finds a face's voxel half a voxel behind its middle and blends corners by the 8
  voxels around them. A merged quad's middle isn't one voxel; with the "same bone, no blend" rule its bone is known
  from the key, emit it with the quad instead.
- tst `body_poses`: counts open edges per face and joint cracks per corner, it'll need to treat a quad's edges as
  spans (or only check meshes that aren't merged).
- `build_smooth_lights_system`: matches by order (above).
- the gpu mesher: same rules, later.
- tst `vox_mesh_grid`: compares the two walkers face for face; compare them after merging too.
- sway (grass): moves vertices by height, a merged quad's corners still sway right (it's per vertex).

### Steps (a commit each, tests passing)

1. Vox models only (colored meshes, no textures, AO from ao27), behind a flag (`zox_greedy_mesh`), so it can be
   compared on and off. Not skeleton meshes yet (they skip it).
2. Tests:
   - `greedy_mesh`: hand made voxes (a flat slab, a step, a crease, two colours side by side): the quad count
     expected (a 16 x 16 slab's top is 1 quad), and rendered with merging on and off, the screenshots identical
     (or the same vertex colour at every face corner, sampled)
   - `vox_instances_stress --gpu --release`: grass triangles and frame time before and after
3. Grass stalks one colour each (a palette index a stalk, not a voxel), so their sides merge: an art choice, check
   with the user first.
4. Skeleton meshes: same bone, no joint blend, bones emitted with the quad; body_poses passing.
5. Terrain: textures repeating in a tile, then the light decision above; perf_fly and terrain_render, dig_light (no
   flash).
6. The gpu mesher, if it's used by then.

### Measuring

- `bash bsh/tst.sh vox_instances_stress --gpu --release`: triangles of a grass block, frame time at 1000 to 16000
- `bash bsh/tst.sh perf_fly --release` (doc/todo.md, Optimizations has the profiling): mesh build time while streaming
- compare runs on the same machine, ollama stopped (it shares the gpu)
