# GPU Meshing

Meshing voxel octrees in a compute shader instead of the cpu mesh builders, with cpu meshing kept as the fallback.
What exists (a tested proof of concept with smooth lighting, 2026-11-14; neighbour chunks, 2026-11-20), what it
measured, and the plan to put it in the game.

Code: [src/lights/lights3/fun/smooth_grid.c](../../src/lights/lights3/fun/smooth_grid.c) (`smooth_grid`: a chunk and
a voxel of border from its 26 neighbours as bytes, shared with cpu smooth lighting),
[src/lights/lights3/fun/gpu.c](../../src/lights/lights3/fun/gpu.c) (`gpu_mesh_smooth_grid`, `gpu_mesh_lighting`),
[src/chunks/chunks3/fun/gpu_mesh.c](../../src/chunks/chunks3/fun/gpu_mesh.c) (the compute and draw shaders, `GpuMesher`),
[src/graphics/opengl/fun/compute.c](../../src/graphics/opengl/fun/compute.c) (`zox_gpu_*` storage buffers, programs,
dispatch, indirect draw), setting `zox_chunks3_gpu_meshing` ([src/chunks/chunks3/set/_.c](../../src/chunks/chunks3/set/_.c)).
Tests: [src/chunks/chunks3/tst/gpu_mesh.c](../../src/chunks/chunks3/tst/gpu_mesh.c),
[src/lights/tst/gpu.c](../../src/lights/tst/gpu.c), [src/lights/tst/gpu_neighbours.c](../../src/lights/tst/gpu_neighbours.c),
[src/models/tst/gpu_mesh_views.c](../../src/models/tst/gpu_mesh_views.c).

## How it works

1. **Grid** (`smooth_grid_fill`, the same one cpu smooth lighting fills): the chunk's voxels and lights at the meshed
   depth as bytes, `(2^depth + 2)³` of them, a voxel of border read from its 26 neighbours (`octree_fill_grid`). A
   node above the depth is its value for its whole cube, as `walk_octree` reads it, so meshing at a lower depth is a
   LOD; lights take an open node's brightest leaf (`getv_nearby_max_LightNode`). Which neighbours have voxels and
   lights goes in two 27 bit masks (`present`, `lights_present`). A 16³ chunk's grid is 5.8KB a tree, a 32³'s 39KB.
   (It replaced flattened octrees, `flatten_octree`, which needed a tree walk a read and had no neighbours: the gpu
   part got several times faster, Measurements.)
2. **Mesh** (`gpu_mesh_grid`): the grids are uploaded to storage buffers, one invocation per voxel reads its voxel and
   its 6 neighbours, across the chunk's border too (`voxel_at`). Each solid voxel face next to air
   claims a slot by adding 6 to an indirect draw command's index count, and writes 4 vertices, 6 indices and 4 vertex
   lights. Vertices are `vec4`: the corner in voxels, `w` packs the voxel, direction, the corner's light and its side of
   the face (`gpu_mesh_vertex_*`).
   - **Lit** (the lights grid and a `GpuMeshLighting` given): each corner by smooth lighting's rule (`smooth_face_lights`):
     the face's, its two edges' and its corner's lights averaged, a corner behind two solid edges is darklight, in a
     chunk without lights sunlight above and darklight elsewhere, solids from a 256 bit mask of the realm's solidity.
   - Each vertex also holds its face's 4 corner lights in one `uint`, so a fragment can blend them across the face.
   - `edge_faces` (default 1): a face into a missing chunk; 0 culls it except the top, as cpu sides do.
   - **Materials** (`gpu_mesher_set_material`): a chunk material's mesh draws only its block material's voxels, a face
     hidden by a neighbour that hides it (`block_material_hides`: solid, or the same material), two 256 bit masks
     (`drawn`, `hides`); block voxes (no cube faces) are neither. All voxels by default (models).
3. **Draw** (`gpu_mesher_draw`): the vertex, light and index buffers are bound as a vertex array and drawn with
   `glDrawElementsIndirect`, the index count read by the gpu from the command. Nothing comes back to the cpu. Colours
   from a palette, each direction a fixed shade, lit by `light_mode`:
   - `gpu_mesh_light_none`: shades only
   - `gpu_mesh_light_face`: a face's 4 corners averaged, flat
   - `gpu_mesh_light_vertex`: each corner's light blended across the face's two triangles, as cpu meshes are lit
   - `gpu_mesh_light_pixel`: the 4 corner lights blended bilinearly across the whole face

- **Sizing by a guess** (2026-11-20): every face is counted (`faces_wanted`, a uint after the draw command), only the
  first `max_faces` are written and drawn, so a mesher too small never draws garbage. A fence after each mesh
  (`zox_gpu_fence`: a sync object on gl, the submission's serial on vulkan) lets `gpu_mesher_counted` read the count
  without waiting, a frame later on gl, ~3 on vulkan; over `max_faces`, `gpu_mesher_grow` makes room for it and a
  quarter more, and it meshes again. The guess for a mesh is its last count; an edit changes few faces, so it nearly
  always fits and draws the frame it's meshed. Chosen over counting first (two passes and a frame before every mesh
  shows). `cells × 3` faces can't overflow (checkers: half its voxels showing all 6), the tests size for it.
- Needs compute: OpenGL ES 3.1+ or OpenGL 4.3+. The game asks for ES 3.0, but drivers give their newest (llvmpipe
  gives ES 3.2, the Khadas' Mali too). `zox_gpu_compute_supported()` checks at runtime, and `zox_gles2`/web builds
  compile it out (`zox_disable_compute`): `gpu_mesher_create` returns 0 and cpu meshing stays.

## In the game (`--gpu_meshing`, off by default)

Solid chunk meshes are meshed on the gpu ([src/lights/lights3/sys/gpu_mesh.c](../../src/lights/lights3/sys/gpu_mesh.c)),
water stays on the cpu. A setting, `zox_chunks3_gpu_meshing`; any test runs with it by adding `--gpu_meshing`.

- A gpu chunk mesh is a `chunk_mesh_gpu` (a variant of the textured one) tagged `GpuMesh`, its own buffers in
  `MeshGPULink` (indices, vertices), `ColorsGPULink` (face lights), `MeshCommandGPULink`, with `MeshFaceRoom` and
  `MeshFence` (rendering/com/gpu_mesh.c). The cpu's mesh build, smooth lights, upload and draw skip it.
- The existing dirty tags drive it, as the cpu's build then upload: `BuildMesh` (voxels) and `BuildMeshColors`
  (lights) fill its grids on worker threads (`chunk_gpu_grid_system`, `chunk_gpu_relight_system`, `zoxp_octree_read`
  beside the cpu build and smooth lights, 3ms a thread a frame) into `GridVoxels`, `GridLights` (byte arrays),
  `GridPresent`, `GridLightsPresent`, then `MeshDirty`: `chunk_gpu_mesh_system` (main thread, `zoxp_gpu_upload`, 2ms a
  frame) uploads and dispatches them, lit by the realm's solidity, its material's masks, and frees them. A mesh's
  main thread part is ~0.03ms (the dispatch ~0.02ms), a streaming burst ~60 meshes a frame.
- `chunk_gpu_mesh_count_system` reads its count once the fence passes: over its room it grows and gets `BuildMesh`,
  else `MeshRenderCount` is its faces and it's `MeshBuilt` (what the lod swap waits for).
- Its terrain holds the compute program and the grid upload buffers (`ComputeGPULink`, `GridGPULink`, each grid sized
  for the deepest grid), made on its first gpu mesh, disposed with it. Without compute its meshes go back to
  the cpu (`GpuMesh` removed).
- Room starts at a face every two voxel columns: in `terrain_gpu_mesh` ~30% of meshes grow once, ~30% of the room is
  used. It never shrinks yet.
- Textured: the compute shader writes each vertex's tilemap uv (the tilemap's `TilemapUVs` uploaded once into its
  `UvsGPULink`, the mesh's uvs in its own `UvsGPULink`), the cpu's uv for the same block, direction and corner.
- `chunk_gpu_render_system` draws them (order 1: after cpu terrain, before water) straight from their buffers with an
  indirect draw: tilemap texel × light (the face's 4 corner lights blended across it), fog. Its terrain holds the draw
  program (`MaterialGPULink`), each mesh its vertex array (`VaoGPULink`).
- `bash bsh/compare_meshing.sh` (`--soft`, `--vulkan`): `terrain_meshing_view` (4 fixed views of a world's terrain,
  no hud) with cpu meshing and with `--gpu_meshing`, a diff and a diff image (2026-11-20: 0.058 gl, 0.071 vulkan on
  the 780M, under 0.08; the same blocks, textures and light).
- Not yet: gpu restore (a lost context), shrinking, water, vox models.

## Tests

- `gpu_mesh_neighbours`: 27 chunks of hills across their borders, the middle one meshed lit from its grid: its faces
  against a voxel-by-voxel reference that looks into the neighbours, every corner light against `smooth_face_lights`
  over the 27; again with three missing (faces into them culled but the top). Fails when the shader ignores its
  neighbours (tried).
- `gpu_mesh_materials`: stone hills, a pond of water and block voxes in one chunk, the stone mesh and the water mesh
  each against a reference with cpu sides' rule (stone shows against water, water hides only water, voxes neither).
- `gpu_mesh_uvs`: every gpu vertex's uv is the tilemap's the cpu takes for that block, direction and corner.
- `terrain_meshing_view` (terrain/tst): the screenshot `bsh/compare_meshing.sh` compares.
- `terrain_gpu_mesh` (terrain/tst): a world streamed with gpu meshing on; once every gpu mesh is built, each one's
  faces against a reference from its chunk's grid (its chunk, depth, material and neighbours), none built on the cpu,
  its draw count its faces. Logs the room used and how many grew.
- `gpu_mesh_grow`: a mesher with room for 16 faces meshes a terrain chunk: its draw has only those 16, the count comes
  back frames later without waiting and is the reference's, it grows, meshes again and has every face.
- `octree_flatten`: 7 shapes (empty, one voxel, solid, sphere, noise, checkers, terrain hills) at depth 5, every voxel
  of the flat array reads back as the octree does, at depth 5, 4 and 3.
- `gpu_mesh`: the same shapes meshed on the gpu at depth 5, 4 and 3, against a voxel-by-voxel reference: the same face
  count, every face in the reference once, its corners a unit square on the right plane, its indices right. Fails when
  a face direction is dropped from the shader. Logs `[metric]` times.
- `gpu_mesh_lights`: every corner light of every gpu face against `smooth_face_lights` for the same voxel, face and
  corner: smooth_lights' corner case, and a terrain chunk (hills in sunlight, a torch lit cave, its light tree reduced)
  at depth 5, 4 and 3. Each vertex's 4 face lights match too. Fails when the corner rule is dropped from the shader.
- `gpu_mesh_views` (`bash bsh/gpu_mesh.sh`, `--visible` to watch on the real gpu): vox models (`zox_tst_vox`, default
  chicken, `zox_tst_gpu_models` of them, default 9) ripple every frame: octree rebuilt, meshed on the gpu from its grid,
  drawn from the gpu, 4 viewports, `tst/gpu_mesh_views.png`. Logs models meshed a second. Checks it meshed every
  second and every view drew them.
- `gpu_light_views` (`bash bsh/gpu_lights.sh`, `--1080p` to see the difference): the same close up into the torch lit
  cave 4 times: unlit, face, vertex, pixel. Logs how many pixels pixel lighting changes from vertex lighting.
- `gpu_light_stress` (`bash bsh/gpu_light_stress.sh`, `--visible` to watch for 20 seconds): terrain chunks at night
  (`zox_tst_gpu_chunks`, default 4) re-shaped and re-lit every frame: the hills roll (voxel octree rebuilt,
  `zox_tst_gpu_voxels=0` keeps them still), two torches move (a light flood and light octree rebuilt), both filled into a grid,
  meshed lit on the gpu, drawn with pixel light. Logs chunks updated a second and each part's time. Checks the last
  animated frame's corner lights against `smooth_face_lights`.

## Measurements

"gpu" times below are upload and mesh unless they say flattening; flattening is on the cpu, in "cpu" times.
Debug builds.

| What | llvmpipe (software gpu) | Mali (the Khadas, `--visible`) |
|---|---|---|
| 32³ terrain chunk: flatten, upload, mesh, wait | ~3ms | |
| the same chunk, cpu sides only (`build_sides_dig`, before its mesh is built) | ~21ms | |
| 32³ terrain chunk lit: flatten voxels and lights, upload, mesh, wait | ~8.3ms (unlit ~3.9ms) | |
| the same chunk, cpu smooth lights only (`smooth_face_lights` for its faces) | ~26ms | |
| a chicken (10×18×12): rebuild its octree and flatten it, cpu | ~0.25ms | ~0.24ms |
| the chicken: upload and mesh (32³ grid), gpu | ~1.2ms | ~0.24ms |
| 9 chickens remeshed every frame | ~130 a second at ~14fps | ~1100 a second at ~125fps |
| 36 chickens remeshed every frame | ~250 a second at ~7fps | |
| `gpu_light_stress`, 4 chunks re-shaped and re-lit every frame | ~25 chunks a second at ~6fps | |
| a chunk of it: cpu voxels / cpu light flood / flatten / gpu upload and mesh | 4.7 / 7.8 / 0.7 / ~9ms | |
| lights only (`zox_tst_gpu_voxels=0`) | ~29 chunks a second, flatten 0.3ms | |

In the light stress the cpu's light flood (a simple one in the test, the game's flood fill is more) is the biggest
cpu part, more than rebuilding the voxels: light flooding on the gpu is the next big step for realtime lights.

With the grid (2026-11-20, llvmpipe, the same tests; the cpu parts ran ~2× faster than the run above too, so compare
within a row's run):

| What | grid | cpu in the same run |
|---|---|---|
| 32³ terrain chunk: fill the grid, upload, mesh, wait | 0.59ms (0.26ms without the fill) | sides only 11.8ms |
| 32³ terrain chunk lit | 0.95ms (unlit 0.58ms) | |
| `gpu_light_stress`, 4 chunks every frame | ~64 chunks a second at ~16fps | |
| a chunk of it: cpu voxels / cpu light flood / grid / gpu upload and mesh | 2.4 / 5.1 / 0.44 / 0.5ms | |
| 9 chickens remeshed every frame | ~324 a second at ~36fps, 0.2ms a chicken | octree 0.08ms |

In the game (2026-11-20, release build, the 780M, opengl, `zox_tst_seed=666 --stress --far 8 --nouis`):

| What | cpu meshing | `--gpu_meshing` |
|---|---|---|
| `perf_stream`: a chunk step settles (average, slowest) | 0.246s, 0.275s | 0.279s, 0.318s |
| `perf_stream`: slowest frame | 9.2ms (`chunk_sides_system` 3.4ms) | 8.9ms (`chunk_gpu_mesh_system` 2.0ms, its budget) |
| `render_fps` far 8 | 378 fps | 368 fps |

- First run the gpu was 0.35s a step, 23ms frames: reading each mesh's count cost ~0.07ms (mapping a buffer in video
  memory), hundreds a frame. Fixed: the count in its own buffer the draws never read, made with a read hint
  (`zox_gpu_storage_buffer_read`, ~2µs a read); the sides of gpu materials aren't built (`chunk_material_gpu`).
- A 4ms dispatch budget didn't settle faster (0.281s) and made 11ms frames: what's left is latency (a frame for the
  count, ~30% of new meshes grow and mesh again), not throughput.
- Drawing a face a voxel costs ~3% fps at far 8 (more triangles than the cpu's merged quads): merging (plan step 7).

So an animated chicken is ~0.5ms a frame on the Mali (half octree and flatten, half gpu), and the frame rate, not
the meshing, was the limit at 9 models.

## The look: what gpu lighting can do better

`gpu_light_views` compares the 4 light modes on the same torch lit cave (`tst/gpu_light_views.png`):

- **Pixel blending removes diagonal seams.** Vertex lighting blends each quad's light across its two triangles, so a
  gradient bends at the diagonal (streaks on lit steps). Bilinear blending across the face doesn't: it changed 14.6% of
  the cave's drawn pixels. It costs nothing on the cpu, the 4 corner lights ride along with each vertex.
- **One face a voxel keeps light detail.** The cpu draws one quad per big node face, lit by only its 4 corners, so
  big flat areas lose their light gradients. The gpu draws a face per voxel, each with its own 4 corners.
- **Next, cheap to try with the tests in place:**
  - a light curve (gamma or a falloff table) in the fragment shader, darklight 16 is linear now
  - ambient occlusion apart from light (a corner's solid count), so caves stay readable when lit
  - per pixel light from the light octree itself (a 3D texture, trilinear), smooth across faces and voxels
  - coloured lights (a light octree per channel, or packed), and lights flooded on the gpu for realtime changes

## Lighting design: what changes light, and what it costs

Decided with the user (2026-11-14). Light stays in the light octrees (flood fill): every entity reads its light from
them, so one lighting path keeps terrain, characters and models lit the same.

| Light | How | When it costs |
|---|---|---|
| Placed torches, glow stones, lava | flood fill into the chunks' light octrees | when placed, broken, edited near, or streamed in |
| A held torch (the player's, the only moving light for now) | the same flood fill, updated as it moves | when it crosses into another voxel |
| The sun | flood fill now (sunlight down open columns) | when blocks change under the sky |

A held torch moving:

1. It crosses into another voxel (not every frame: a few times a second at walking speed).
2. Dark flood its old light out, flood its new light in (the existing dark and light queues), in the few chunks its
   reach touches (255 losing `light_air_decay` 32 a step is ~7 voxels: its chunk and maybe a neighbour or two).
3. Those chunks re-mesh lights only: their voxel nodes stay uploaded, only their flat light octrees are uploaded, the
   mesh is rerun (`gpu_mesh_flat_lit`, ~1.9ms a chunk on the Mali in `gpu_light_stress`, less batched).

`gpu_light_stress` is the worst case on purpose (every chunk re-lit from scratch every frame): on the Mali a chunk's
gpu part was ~1.9ms and its cpu part ~12.8ms (debug build: 4.5ms voxels, 7.7ms a whole chunk light flood, 0.6ms
flattening), 68 chunk updates a second. A held torch needs a few chunks a move, and an incremental flood around one
torch is a small part of a whole chunk's.

Later, if wanted:
- **A held torch test:** `gpu_light_stress` with one torch moving at walking speed across chunk borders, re-lighting
  only the chunks it touches with an incremental flood, logging chunks a move and time a move.
- **Day and night without re-lighting:** keep sky light (how open a spot is to the sky) and block light (torches) as
  two values, the shader takes `max(sky × sun_brightness, block)`, a sunset is one uniform. Today one light value
  holds both, so a darker sky would mean re-flooding every chunk.
- **Flooding on the gpu:** for streaming many lit chunks in at once (a town's glow stones), many dirty chunks flooded
  in one dispatch, the light grid never leaving the gpu.

## How it differs from the cpu path

- **Chunk borders:** both read the neighbours' voxels and lights. The gpu reads a neighbour at the meshed depth (its
  grid), the cpu's sides check a more detailed neighbour's every voxel on that side (`get_node_sides_all_hide`): at a
  border between lods the gpu may cull a face the cpu shows (step 5).
- **Big nodes:** the cpu decides a big node's faces from neighbour nodes' values (`adjacent_node->value`, the mode),
  so a face partly next to air can be culled. The gpu checks every voxel.
- **Merging:** the cpu draws one quad per big node face, the gpu one per voxel face: more vertices for solid areas,
  more light detail.
- **Light across a face:** blended bilinearly (no diagonal seam), the cpu's across its two triangles.

## Plan for the game

Each step behind `zox_chunks3_gpu_meshing` and `zox_gpu_compute_supported()`, the cpu path unchanged, a test first.

1. **Neighbours** (done, 2026-11-20): the shader reads smooth lighting's grid, a voxel of border from the 26
   neighbours (`gpu_mesh_neighbours`).
2. **Batching.** Many dirty chunks in one dispatch: one big nodes buffer with an offset per chunk, one command per
   chunk (`glMultiDrawElementsIndirect` on GL, a loop of indirect draws on ES). "Per terrain" would be this, the
   meshes stay per chunk.
3. **Memory.** Sized to the mesh's faces (done, `gpu_mesh_grow`). Later, smaller: pack a face into a few `uint`s
   (position 5+5+5 bits, direction, voxel, lights) and build corners in the vertex shader (memory only, not speed).
4. **Textures.** Pass the tilemap's uvs as a uniform or buffer, the vertex shader picks them by voxel and direction,
   drawn with the terrain's textured shader (and its pixel light blend).
5. **LODs and the swap.** Mesh at the chunk's `RenderDepth` (the depth uniform, light reads are already coarse maxes).
   The mesh is ready the frame after its dispatch; the swap (doc/done/streaming.md) can wait on a `glFenceSync`.
6. **Lights on the gpu.** A light change remeshes only lights (see Lighting design): keep the voxel nodes uploaded, upload the light nodes,
   rerun. Later flood light on the gpu too.
7. **Merging** once the rest matches, keeping per pixel light detail (merge faces with equal corner lights only).

Animation: `gpu_mesh_views` shows a model's octree rebuild and flatten is as cheap as its gpu mesh (~0.24ms each on
the Mali for a chicken). Animated models could also skip the octree: a dense 3D texture or a voxel buffer written by a
compute "animation" pass.

## Notes

- Raycasts, physics and collisions read the octrees, not meshes, so they don't care which path meshed a chunk.
- The compute and draw shaders are strings in `fun/gpu_mesh.c`: the shader loader adds `#version 300 es`, compute
  needs `310 es`. Move them to `res/shaders` once the loader takes a version per file.
- `zox_gpu_storage_read` maps and waits for the gpu: for tests and counters only, never per frame in the game.
- `packed` is a reserved word in GLSL (a compile error on mesa).
