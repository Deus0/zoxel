# Chunks Refactor: one way to mesh voxels

Terrain chunks and vox models (characters, items, minivoxes, critters) are both voxel octrees made into meshes, but
by two meshers that share almost nothing. The goal: one set of parts for every chunk3, so terrain's features (block
materials, see through meshes, lods side by side, many chunks under one parent) are every vox model's too, a boss
made of several chunks included, and each part is tested once.

Code: [src/chunks/chunks3](../../src/chunks/chunks3) (`sys/build_colored.c`, `sys/build_textured.c`, `sys/sides.c`,
`sys/vox_lods.c`, `fun/materials.c`, `fun/gpu_mesh.c`), [src/lights/lights3](../../src/lights/lights3)
(`fun/smooth_grid.c`, `sys/gpu_mesh.c`, `sys/build.c`, `sys/build_smooth.c`). Related: [gpu_meshing.md](gpu_meshing.md),
[chunk_lods.md](chunk_lods.md), [doc/tech/chunks3.md](../tech/chunks3.md) (chunk3 parents).

## Today (2026-11-25)

| | Terrain chunks (textured) | Vox models (colored) |
|---|---|---|
| Parent | a chunk3 parent: terrain (`prefab_chunk3_parent`) | none: a vox is one chunk on its own |
| Many chunks | yes, by position (`ChunkLinks`), neighbours (`ChunkNeighbors`) | no |
| What a voxel is | a block (`BlockManagerLink`), its tile (`Tilemap`) | a palette colour (`ColorRGBs`) |
| Materials | a `ChunkMaterial` child per block material: solid, liquid, oil (`fun/materials.c`) | one mesh, everything solid |
| See through | liquids: `TransparentMesh`, their own texture | no |
| Lods | a `ChunkMesh` per lod under each material, swapped (`sys/transition.c`), mixed lods side by side (`sys/sides.c`) | a `VoxLodMesh` child per lod (`sys/vox_lods.c`) |
| Which faces show | read from a prebuilt `SidesOctree` per material (`chunk_sides_system`) | worked out per voxel: `is_adjacent_all_solid`, or a pyramid grid (`build_voxel_mesh_grid`) |
| Face corners | `voxel_face_vertices_n` (blocks/dat/mesh2.c) | `voxel_face_vertices_v2` (chunks3/dat/mesh.c), the same values |
| Writing a face | `zox_build_voxel_face`: `corner * scale + position` | `add_voxel_face`: `(corner + position) * scale + offset` |
| Walking the octree | `zox_terrain_building_dig` | `build_voxel_mesh_c`, `build_voxel_mesh_pyramid_dig` |
| Per vertex | tilemap uvs, white, then lit by smooth lights (`build_smooth.c`) | palette colour darkened by ambient occlusion (`fun/ao27.c`) |
| GPU meshing | yes, solid materials (`lights3/sys/gpu_mesh.c`), faces from a voxel grid, no `SidesOctree` | not in the game (a test meshes models on the gpu: tst gpu_mesh_views) |

A grid of voxels already decides which faces show in three places: the gpu shader, `gpu_mesh_grid_faces` (the cpu
counting a gpu mesh's faces), and the colored pyramid. `SidesOctree` is the odd one out: a system, its triggers and
the `BuildChunkSides` waits (behind several lod seam races), for cpu textured meshing and cpu lights only.

## The target

- **Every chunk3 has the same shape:** chunk3 > a `ChunkMaterial` child per material its voxels have > a `ChunkMesh`
  child per lod. A vox model is a chunk3 like a terrain chunk, its lods and materials meshed and swapped the same way.
- **Every model can be many chunks:** a chunk3 parent holds them by position; one chunk is the common case, a boss
  can be several. Faces between its chunks are hidden as terrain's are, lods mix as terrain's do.
- **What a voxel is, linked like everything else (ecs):** a textured parent links its blocks and tilemap (done); a
  colored one links its material entities (each a colour and a `BlockMaterial`: solid, liquid, see through, emissive
  later), and its only component is the palette, a byte array: a voxel value to one of its linked materials. Today the
  colours themselves are the component (`ColorRGBs`). Either kind, meshing and lighting read a voxel value's material
  through the same lookup, built once a build as the textured mesher builds its solidity table.
- **One face rule:** the padded voxel grid (`smooth_grid`: the chunk at the meshed depth plus a voxel of border from
  its 26 neighbours at the depth each draws), on the cpu and in the gpu shader. No `SidesOctree`.
- **One cpu mesher:** one octree walk to the target depth, one face writer, the rule above; the per vertex step is
  the only difference: uvs (textured) or palette colour with ambient occlusion (colored).
- **GPU meshing for both,** the cpu mesher kept as the fallback (no compute: ES 3.0, web) and as the reference tests
  compare the gpu's against. Both are options (a setting, as `zox_chunks3_gpu_meshing` now): cpu for low end machines,
  gpu for high end, every feature in both.
- **Buffers stay in flecs:** a mesher's working memory (a grid, its arrays) belongs to the system run that meshes, or
  to a component: no globals, no static or thread local buffers outside them. A system allocates once a run and
  reuses it for its entities (a function takes the buffer it fills).

## Rules for every step

- A commit per step; each builds on every platform and passes its tests. Steps 1 to 3 change nothing drawn: a test
  compares the old output with the new, face for face, before the old code goes.
- Every structural change (new components, a new link, tags) is agreed before its step starts.
- Measure before and after each step: tst `perf_meshing` (both cpu meshers on the same voxels, ms a mesh and its
  faces, debug and `--release`; the baseline is in doc/results.md, Meshing speed), and `perf_terrain` (cpu and gpu
  meshing) for a phase. No step may be slower than the baseline; its face counts are checked, so a step that draws
  other faces fails it until the counts are agreed and updated.

## Phase 1: shared parts, nothing drawn changes

1. **One face table:** remove `voxel_face_vertices_v2`, colored uses `voxel_face_vertices_n`. Check: tst
   `vox_mesh_grid`, `model_views`, `chunk_uvs`.
2. **One face writer:** `mesh_add_face(indices, vertices, direction, position, scale)` writes a face's 6 indices and
   4 vertices; both meshers call it, then add their own per vertex data. Keep each face's 4 vertices in order:
   skeleton weights read meshes as 4 vertices a face with the voxel behind it (bones/sys/generate.c).
3. **One octree walk:** one dig to the target depth calling a per voxel step; the three digs go. Check: the same
   tests, and a new tst `mesher_walk`: a small octree with nodes above the target depth (closed and open), each
   walker's faces before against after.

## Phase 2: one face rule, no SidesOctree

1. **A cpu textured mesher from the grid:** fill the smooth grid as the gpu does (`smooth_grid_fill`,
   `smooth_grid_borders` for neighbours drawn at other depths), its faces by the shader's rule
   (`gpu_mesh_grid_faces` is that rule on the cpu, its face count). Behind a setting at first.
2. **Prove it:** tst `grid_sides`: every chunk of a streamed walk (stream_seams' path) meshed both ways, the same
   faces (position, direction, material). Then the grid mesher is the default.
3. **CPU lights** walk faces from the grid (or the mesh's own faces) instead of `SidesOctree`
   (`lights3/sys/build.c`, `build_smooth.c`).
4. **Remove** `SidesOctree`, `SidesOctreeLock`, `chunk_sides_system`, its triggers and the `BuildChunkSides` waits.
   Check: stream_seams, chunk_mesh_flicker, lod_swap, lod_light_flash, chunk_uvs, terrain_gpu_mesh, perf_terrain.
5. **Colored from the same grid:** fill it with no neighbours (a model alone) instead of the pyramid. Models deeper
   than the grid allows (depth 8 is a 258³ grid, 17MB) keep a walk, or mesh a grid per sub cube later.

## Phase 3: vox models shaped like terrain chunks

1. **Materials for colored voxels:** material entities linked from the colored parent (as a textured one links its
   blocks), each its colour and `BlockMaterial`; the palette a byte array component, a voxel value to a linked
   material, replacing `ColorRGBs`. A model made from a .vox gets one material entity a colour, all solid; materials
   are shared between models where they're the same (as blocks are by realm). Agree the link's and the palette's
   names before this step.
2. **Vox meshes under materials:** a vox's `VoxLodMesh` children become `ChunkMaterial` > `ChunkMesh` per lod, as
   terrain's (`fun/materials.c`, `sys/mesh_spawn.c`, `sys/transition.c` for swaps). A see through material draws
   blended (`TransparentMesh`) after the solid ones.
3. **Colored chunk3 parents:** `spawn_chunk3_parent_colored(world, prefab, realm, materials, depth, scale)` beside
   the textured one; its chunks by position, their neighbours linked, faces between them hidden, for models bigger
   than a chunk (bosses).
4. **Single chunks stay single:** a model that fits one chunk is a chunk3 with no parent, as vox models are now, cheap
   for the thousands of minivoxes, items and grass tufts: `chunk3_single` (textured) and `chunk3_single_colored`
   prefabs, meshed and lit by the same parts, no neighbours (the grid's border is air). It links its own materials
   and palette, as a parent would.
5. **What uses vox models:** characters (combined from parts, `sys/combine.c`, then boned: weights per vertex),
   items, minivoxes, critters, block voxes. Each checked by its tests as its meshes move under materials.

## Phase 4: GPU meshing for vox models

1. The shader's per vertex step for colored: its material's colour (a buffer of the palette's colours) and ambient occlusion by
   `fun/ao27.c`'s rule (the grid has the 27 neighbours).
2. Materials as terrain's: `gpu_mesher_set_material` already draws one material's voxels.
3. Skinned meshes (characters), decided 2026-11-25: skinned in the shader, the bone octree uploaded too: the shader
   writes each vertex's voxel (it already packs it in `w`) and its bones come from the bone octree by voxel, as
   bones/sys/generate.c does on the cpu.
4. Lods and swaps as gpu terrain chunks have them.

## Tests when it's done

Each part is checked once, at the lowest level that has it; the big walks only check that the parts meet:

| Part | Test |
|---|---|
| The face rule (which faces show), lods mixed, borders | one test on given grids: the cpu mesher and the gpu mesher the same faces; neighbours at other depths |
| The walk and face writer | `mesher_walk` (Phase 1) |
| Meshing speed, both kinds | `perf_meshing` (its gpu meshing too from phase 4) |
| Textured per vertex | `chunk_uvs`: every regular block's faces their tile for their direction, liquids their own texture |
| Colored per vertex | a colour and ambient occlusion test: a small model, each corner's colour by the rule |
| Lights per vertex | `smooth_lights`, `lod_lights`, `water_light` |
| Materials | a chunk with each material: its own mesh, see through ones blended (terrain and vox alike) |
| Many chunks | a colored chunk3 parent of 2 x 2 x 2: no faces between its chunks, faces at its outside |
| Streaming, lod swaps, seams in play | `stream_seams`, `chunk_mesh_flicker`, `lod_light_flash`, `perf_terrain` |

Retired as they're covered: `chunk_sides` (by the face rule test), `stream_sides` and `stream_gpu_sides` (each
rebuilt its own reference of a rule: by stream_seams and the cpu against gpu test), `terrain_gpu_mesh`'s reference,
`vox_mesh_grid` (one walker left).

## Open questions

- Decided (2026-11-25): vox materials are linked entities like blocks, the palette (a byte array) the one component.
- Decided (2026-11-25): a single vox model is a chunk3 with no parent (`chunk3_single`, `chunk3_single_colored`), no
  refactor to parents for it.
- Decided (2026-11-25): colored models are lit as block voxes are now, their ambient occlusion colours times one
  `Brightness` from the world's light where they are (vodes3/fun/lights.c, `get_block_vox_brightness`). Better model
  lighting (smooth lights across a model) is a prototype after this refactor.
- Faces merged (greedy meshing, doc/tech/chunks3.md) after this, in the one mesher.

## Progress (2026-11-25)

Phase 2 done, committed:
- The game meshes and lights cpu terrain from the grid (fd2b0145e). perf_terrain release far 8, 4 runs each
  alternated: frames over 11ms ~350 to ~100 a stream, 1% low 45 to 48 fps, steps 1.8s to 1.5s
  (doc/tech/fps_optimization.md). tst perf_meshing checked the grid's faces against sides' first (24ebd58c1).
- The raycast gizmo's faces from the voxels (f5628ccbc, tst block_sides): it outlined nothing with gpu meshing.
- SidesOctree removed (f2084a0db): its component, lock, build_sides_dig and the sides walks; chunk_sides_system only
  adds BuildMesh to a chunk's meshes; chunk_sides ported to the grid; stream_sides, stream_gpu_sides gone. The side
  children table (get_node_sides_all_hide, colored meshes) is filled at import: perf_meshing's face counts caught
  colored models at lower lods drawing half their faces without it (vox_mesh_grid and model_views didn't).

Left of phase 2:
- The `BuildChunkSides` waits: the mesher and lights skip a chunk with it, a frame later chunk_sides_system turns it
  into BuildMesh. A rebuild could be asked for directly (BuildMesh on the meshes), the tag and system gone; measure
  perf_terrain before and after (the waits held seams back, chunk_mesh_flicker and stream_seams must stay clean).
- The tag's name: it means "rebuild against the neighbours" now.

Phase 1:
- 1 done (eb5d10b5d): one face table, voxel_face_vertices_v2 gone.
- 2 done (1ecb7afc8): one face writer, mesh_add_face (fun/faces.c), both meshers add their uvs or colours after.
- 3 done: one colored walk, build_voxel_mesh (sys/build_colored.c): one dig, its neighbours read through one lookup,
  the pyramid (models up to 128³, an array read) or the octree (deeper), neighbour chunks' trees past the border either
  way. A model with no neighbour chunks takes the pyramid's own face step (through the lookup it was 10% slower).
  Release, 32³: the pyramid 0.90ms (as before), the walk 4.07ms (4.35 before).

Then Phase 3 (vox models shaped as terrain chunks).

## Results

- Phase 2, 1 to 3 (2026-11-25): the game meshes and lights cpu terrain from the grid (zox_chunks3_grid_meshing,
  `--sides_meshing` the old way); sides aren't built, chunk_sides_system only triggers rebuilds. perf_terrain far 8
  debug: load 11.1s to 6.8s, stream steps 8.6s to 7.0s (doc/results.md). Next: remove SidesOctree and its system.

- Baseline (2026-11-25): doc/results.md, Meshing speed. A textured chunk's sides are ~85% of its cpu cost; the
  colored pyramid grid is 4 to 5 times the octree walk at full detail.
