# GPU Meshing

Meshing voxel octrees in a compute shader instead of the cpu mesh builders, with cpu meshing kept as the fallback.
What exists (a tested proof of concept with smooth lighting, 2026-11-14), what it measured, and the plan to put it in
the game.

Code: [src/_/octrees/flatten.c](../../src/_/octrees/flatten.c) (any octree → flat array),
[src/chunks/chunks3/fun/flatten.c](../../src/chunks/chunks3/fun/flatten.c) (`flatten_VoxelNode`),
[src/lights/lights3/fun/gpu.c](../../src/lights/lights3/fun/gpu.c) (`flatten_LightNode`, `gpu_mesh_lighting`),
[src/chunks/chunks3/fun/gpu_mesh.c](../../src/chunks/chunks3/fun/gpu_mesh.c) (the compute and draw shaders, `GpuMesher`),
[src/graphics/opengl/fun/compute.c](../../src/graphics/opengl/fun/compute.c) (`zox_gpu_*` storage buffers, programs,
dispatch, indirect draw), setting `zox_chunks3_gpu_meshing` ([src/chunks/chunks3/set/_.c](../../src/chunks/chunks3/set/_.c)).
Tests: [src/chunks/chunks3/tst/gpu_mesh.c](../../src/chunks/chunks3/tst/gpu_mesh.c),
[src/lights/tst/gpu.c](../../src/lights/tst/gpu.c),
[src/models/tst/gpu_mesh_views.c](../../src/models/tst/gpu_mesh_views.c).

## How it works

1. **Flatten** (`flatten_octree`): the octree becomes one `uint` array, breadth first. A node is its value (bits 0-7)
   and the index of its first child (bits 8-31, 0 for none), its 8 children in a row in octree order
   (`x | y << 1 | z << 2`). A node without children is its value for its whole cube, as `walk_octree` reads it, so
   meshing at a lower depth is a LOD. Linked nodes are leaves. Light trees flatten with `max_values`: an open node holds
   its leaves' brightest, as the cpu's coarse reads (`getv_nearby_max_LightNode`). A 32³ terrain-like chunk is ~7.7k
   nodes (~30KB).
2. **Mesh** (`gpu_mesh_flat_lit`): the nodes are uploaded to a storage buffer, one invocation per voxel walks them to
   its voxel and its 6 neighbours (`voxel_at`, the same walk as `walk_octree`). Each solid voxel face next to air
   claims a slot by adding 6 to an indirect draw command's index count, and writes 4 vertices, 6 indices and 4 vertex
   lights. Vertices are `vec4`: the corner in voxels, `w` packs the voxel, direction, the corner's light and its side of
   the face (`gpu_mesh_vertex_*`).
   - **Lit** (light nodes and a `GpuMeshLighting` given): each corner by smooth lighting's rule (`smooth_face_lights`):
     the face's, its two edges' and its corner's lights averaged, a corner behind two solid edges is darklight, past
     the chunk sunlight above and darklight elsewhere, solids from a 256 bit mask of the realm's solidity.
   - Each vertex also holds its face's 4 corner lights in one `uint`, so a fragment can blend them across the face.
   - `edge_faces` (default 1): 0 culls border faces except the top, as the cpu does with no neighbour chunk.
3. **Draw** (`gpu_mesher_draw`): the vertex, light and index buffers are bound as a vertex array and drawn with
   `glDrawElementsIndirect`, the index count read by the gpu from the command. Nothing comes back to the cpu. Colours
   from a palette, each direction a fixed shade, lit by `light_mode`:
   - `gpu_mesh_light_none`: shades only
   - `gpu_mesh_light_face`: a face's 4 corners averaged, flat
   - `gpu_mesh_light_vertex`: each corner's light blended across the face's two triangles, as cpu meshes are lit
   - `gpu_mesh_light_pixel`: the 4 corner lights blended bilinearly across the whole face

- Buffers are sized for `cells × 3` faces, the most a chunk can have (checkers: half its voxels showing all 6), so a
  mesh can't overflow.
- Needs compute: OpenGL ES 3.1+ or OpenGL 4.3+. The game asks for ES 3.0, but drivers give their newest (llvmpipe
  gives ES 3.2, the Khadas' Mali too). `zox_gpu_compute_supported()` checks at runtime, and `zox_gles2`/web builds
  compile it out (`zox_disable_compute`): `gpu_mesher_create` returns 0 and cpu meshing stays.

## Tests

- `octree_flatten`: 7 shapes (empty, one voxel, solid, sphere, noise, checkers, terrain hills) at depth 5, every voxel
  of the flat array reads back as the octree does, at depth 5, 4 and 3.
- `gpu_mesh`: the same shapes meshed on the gpu at depth 5, 4 and 3, against a voxel-by-voxel reference: the same face
  count, every face in the reference once, its corners a unit square on the right plane, its indices right. Fails when
  a face direction is dropped from the shader. Logs `[metric]` times.
- `gpu_mesh_lights`: every corner light of every gpu face against `smooth_face_lights` for the same voxel, face and
  corner: smooth_lights' corner case, and a terrain chunk (hills in sunlight, a torch lit cave, its light tree reduced)
  at depth 5, 4 and 3. Each vertex's 4 face lights match too. Fails when the corner rule is dropped from the shader.
- `gpu_mesh_views` (`bash bsh/gpu_mesh.sh`, `--visible` to watch on the real gpu): vox models (`zox_tst_vox`, default
  chicken, `zox_tst_gpu_models` of them, default 9) ripple every frame: octree rebuilt, flattened, meshed on the gpu,
  drawn from the gpu, 4 viewports, `tst/gpu_mesh_views.png`. Logs models meshed a second. Checks it meshed every
  second and every view drew them.
- `gpu_light_views` (`bash bsh/gpu_lights.sh`, `--1080p` to see the difference): the same close up into the torch lit
  cave 4 times: unlit, face, vertex, pixel. Logs how many pixels pixel lighting changes from vertex lighting.
- `gpu_light_stress` (`bash bsh/gpu_light_stress.sh`, `--visible` to watch for 20 seconds): terrain chunks at night
  (`zox_tst_gpu_chunks`, default 4) re-shaped and re-lit every frame: the hills roll (voxel octree rebuilt,
  `zox_tst_gpu_voxels=0` keeps them still), two torches move (a light flood and light octree rebuilt), both flattened,
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

## How it differs from the cpu path

- **Chunk borders:** the cpu reads neighbour chunks' voxels and lights. The gpu has no neighbours yet: past the edge
  is air (or culled with `edge_faces` 0), and its light is sunlight above, darklight elsewhere.
- **Big nodes:** the cpu decides a big node's faces from neighbour nodes' values (`adjacent_node->value`, the mode),
  so a face partly next to air can be culled. The gpu checks every voxel.
- **Merging:** the cpu draws one quad per big node face, the gpu one per voxel face: more vertices for solid areas,
  more light detail.
- **Not done:** textures (terrain uses the tilemap's uvs), transparent blocks beyond solidity, block voxes.

## Plan for the game

Each step behind `zox_chunks3_gpu_meshing` and `zox_gpu_compute_supported()`, the cpu path unchanged, a test first.

1. **Neighbours.** Updates stay per chunk. Give the shader its 6 neighbours' voxels and lights (26 for smooth light
   corners): upload them flattened in the same buffers with an offset each, `voxel_at` and `light_at` step into a
   neighbour past the edge. Test: `chunk_sides` and `smooth_lights` cases, gpu against cpu, face for face and light for
   light.
2. **Batching.** Many dirty chunks in one dispatch: one big nodes buffer with an offset per chunk, one command per
   chunk (`glMultiDrawElementsIndirect` on GL, a loop of indirect draws on ES). "Per terrain" would be this, the
   meshes stay per chunk.
3. **Memory.** `cells × 3` faces is ~6MB of vertices a 32³ chunk, too much for a terrain. Pack a vertex into one or two
   `uint`s (position 5+5+5 bits, direction, voxel, light, side) and build corners in the vertex shader, or count first
   then allocate (a count pass, a prefix sum, a write pass) into one shared buffer.
4. **Textures.** Pass the tilemap's uvs as a uniform or buffer, the vertex shader picks them by voxel and direction,
   drawn with the terrain's textured shader (and its pixel light blend).
5. **LODs and the swap.** Mesh at the chunk's `RenderDepth` (the depth uniform, light reads are already coarse maxes).
   The mesh is ready the frame after its dispatch; the swap (doc/tec/streaming.md) can wait on a `glFenceSync`.
6. **Lights on the gpu.** A light change remeshes only lights: keep the voxel nodes uploaded, upload the light nodes,
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
