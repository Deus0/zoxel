# Chunks3: Voxel Meshes

How voxel octrees (terrain chunks, vox models, characters) become meshes, and the plan for greedy meshing. Code in
[src/chunks/chunks3](../../src/chunks/chunks3), lighting in [src/lights/lights3](../../src/lights/lights3).

## Chunk3 parents

A chunk3 parent is a vox whose children are chunks, found by position (`ChunkLinks`), meshed and lit together
([pre/chunk3_parent.c](../../src/chunks/chunks3/pre/chunk3_parent.c), [ins/chunk3_parent.c](../../src/chunks/chunks3/ins/chunk3_parent.c)):

- `spawn_chunk3_parent_textured(world, prefab, realm, blocks, tilemap, depth, scale)`, then
  `spawn_chunk3_parent_chunk(world, prefab, parent, position, render_depth)` for each chunk. Chunks with lights are
  `prefab_chunk_lit` (lights3/fun/prefab.c).
- What its voxels are is linked on it: its blocks (`BlockManagerLink`, a realm's `BlockLinks`) and their tiles
  (`Tilemap`); its chunks take both from it. A colored parent (vox models) would link its palette the same way.
- Chunks side by side at different lods are its job: the faces between them (sys/sides.c), the light across them
  (lights3), the mesh swapped as a chunk's lod changes (sys/transition.c).
- A chunk's voxels done (`GenerateChunk` at `zox_generate_terrain_sunlight`): it's lit, then meshed
  (lights3/sys/generated.c). `SunnyChunk` on the top layer takes the sky's light, `BottomChunk` on the bottom ends beams.
- Terrain is a chunk3 parent that streams its chunks round its streamers, generates them and picks their lods
  (`prefab_terrain`, `prefab_chunk_terrain`).
- tst: `zox_tst_chunk3_parent_ready` (lights/tst/chunk3_parent.c), a realm and 3 x 3 x 3 lit chunks (tst water_light).

## How meshes are built now

Every mesh is one quad (4 vertices, 6 indices) per visible voxel face. Nothing is merged: a flat 16 x 16 floor is
256 quads on its top alone. Vertices aren't shared between quads.

### Vox models (colored meshes)

[sys/build_colored.c](../../src/chunks/chunks3/sys/build_colored.c), entry [sys/vox_lods.c](../../src/chunks/chunks3/sys/vox_lods.c)
(`build_vox_mesh_at_depth`): minivoxes, grass, items, characters' bodies (skeleton meshes), each a mesh a lod.

- Two walkers, the same output (tst `vox_mesh_grid` checks they match face for face, in order):
  - `build_voxel_mesh_grid`: the octree flattened into a padded grid of node pointers a depth (a pyramid), so a
    neighbour is an array read; used up to `zox_vox_grid_max_depth` (7)
  - `build_voxel_mesh_c`: the octree walk, for deeper voxes and chunks with neighbours
- A voxel's face shows if its neighbour that way isn't solid (`is_adjacent_all_solid`).
- Colour is baked a vertex: the voxel's palette colour darkened by **ambient occlusion**:
  - `zox_ambient_occlusion27` (on): each face corner looks at 3 of the voxel's 27 neighbours, its two side
    neighbours and the corner between them ([fun/ao27.c](../../src/chunks/chunks3/fun/ao27.c), `neighbor_indices_27`,
    `compute_ao_factor_27`: both sides solid is full corner darkness, else darker the more are solid)
  - else [fun/ao6.c](../../src/chunks/chunks3/fun/ao6.c): the 6 face neighbours only
  - so one voxel's face can have 4 different corner colours; that's the shading in creases
- Skeleton meshes: each vertex gets bones by the voxel behind its face (`generate_weights_parts`,
  [bones/sys/generate.c](../../src/bones/sys/generate.c)), it reads meshes as **4 vertices a face** in order, and
  blends a corner between joined bones (doc/done/skeletons.md).

### Terrain chunks (textured meshes)

[sys/build_textured.c](../../src/chunks/chunks3/sys/build_textured.c) (`chunk_textured_build_system`,
`zox_terrain_building_dig`), a mesh per chunk material (solid, water: [fun/materials.c](../../src/chunks/chunks3/fun/materials.c)).

- Which faces show is worked out first, into a `SidesOctree` ([sys/sides.c](../../src/chunks/chunks3/sys/sides.c)),
  across chunk borders and lods (tst `chunk_sides`).
- UVs: each face gets its block's tile in the tilemap atlas (`tilemap_uvs`, 4 corners a face), liquids their own
  whole texture.
- Light is baked a vertex **after** the mesh, by `build_smooth_lights_system`
  ([lights3/sys/build_smooth.c](../../src/lights/lights3/sys/build_smooth.c)): a vertex averages the light of the
  face's air voxel, its two edges' and its corner's, a corner behind two solid edges counts as dark. That is the
  terrain's ambient occlusion. It walks the voxels and sides **in the same order as the mesher** and writes 4 colours
  a face, so its colours line up with the mesh's vertices by position in the arrays. The upload waits for it
  (`BuildMeshColors`), else a new mesh shows a frame white.

### GPU meshing (proof of concept)

[fun/gpu_mesh.c](../../src/chunks/chunks3/fun/gpu_mesh.c), doc/plans/gpu_meshing.md: a compute shader does the same
(a quad a face, smooth light corners), no textures or neighbours yet, no merging.

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
