# GPU Meshing

Meshing voxel octrees in a compute shader instead of the cpu mesh builders, with cpu meshing kept as the fallback.
What exists (a tested proof of concept, 2026-11-14), what it measured, and the plan to put it in the game.

Code: [src/chunks/chunks3/fun/flatten.c](../../src/chunks/chunks3/fun/flatten.c) (octree → flat array),
[src/chunks/chunks3/fun/gpu_mesh.c](../../src/chunks/chunks3/fun/gpu_mesh.c) (the compute and draw shaders, `GpuMesher`),
[src/graphics/opengl/fun/compute.c](../../src/graphics/opengl/fun/compute.c) (`zox_gpu_*` storage buffers, programs,
dispatch, indirect draw), setting `zox_chunks3_gpu_meshing` ([src/chunks/chunks3/set/_.c](../../src/chunks/chunks3/set/_.c)).
Tests: [src/chunks/chunks3/tst/gpu_mesh.c](../../src/chunks/chunks3/tst/gpu_mesh.c),
[src/models/tst/gpu_mesh_views.c](../../src/models/tst/gpu_mesh_views.c).

## How it works

1. **Flatten** (`flatten_VoxelNode`): the octree becomes one `uint` array, breadth first. A node is its value (bits
   0-7) and the index of its first child (bits 8-31, 0 for none), its 8 children in a row in octree order
   (`x | y << 1 | z << 2`). A node without children is its value for its whole cube, as `getv_VoxelNode` reads it, so
   meshing at a lower depth is a LOD. Linked nodes are leaves. A 32³ terrain-like chunk is ~7.7k nodes (~30KB).
2. **Mesh** (`gpu_mesh_flat`): the nodes are uploaded to a storage buffer, one invocation per voxel walks them to its
   voxel and its 6 neighbours (`voxel_at`, the same walk as `walk_octree`). Each solid voxel face next to air or the
   chunk's edge claims a slot by adding 6 to an indirect draw command's index count, and writes 4 vertices and 6
   indices. Vertices are `vec4`: the corner in voxels, `w = voxel + direction * 256`.
3. **Draw** (`gpu_mesher_draw`): the vertex and index buffers are bound as a vertex array and drawn with
   `glDrawElementsIndirect`, the index count read by the gpu from the command. Nothing comes back to the cpu.
   No lights: each direction a fixed shade, colours from the model's palette.

- Buffers are sized for `cells × 3` faces, the most a chunk can have (checkers: half its voxels showing all 6), so a
  mesh can't overflow.
- Needs compute: OpenGL ES 3.1+ or OpenGL 4.3+. The game asks for ES 3.0, but drivers give their newest (llvmpipe
  gives ES 3.2, Mali G52-class drivers 3.2). `zox_gpu_compute_supported()` checks at runtime, and `zox_gles2`/web builds
  compile it out (`zox_disable_compute`): `gpu_mesher_create` returns 0 and cpu meshing stays.

## Tests

- `octree_flatten`: 7 shapes (empty, one voxel, solid, sphere, noise, checkers, terrain hills) at depth 5, every voxel
  of the flat array reads back as the octree does, at depth 5, 4 and 3.
- `gpu_mesh`: the same shapes meshed on the gpu at depth 5, 4 and 3, against a voxel-by-voxel reference: the same face
  count, every face in the reference once, its corners a unit square on the right plane, its indices right. Checked
  that it fails when a face direction is dropped from the shader. Logs `[metric]` times.
- `gpu_mesh_views` (`bash bsh/gpu_mesh.sh`, `--visible` to watch on the real gpu): vox models (`zox_tst_vox`, default
  chicken, `zox_tst_gpu_models` of them, default 9) ripple every frame: octree rebuilt, flattened, meshed on the gpu,
  drawn from the gpu, 4 viewports, `tst/gpu_mesh_views.png`. Logs models meshed a second each second. Checks it meshed
  every second and every view drew them.

## Measurements

On the Khadas with llvmpipe (a software gpu running on the cpu) and a debug build, so the gpu numbers are an upper
bound; measure on the real gpu (`--gpu` or `--visible`, ask the user first, mali once crashed gnome-shell).

| What | Time |
|---|---|
| gpu mesh, 32³ terrain chunk (flatten, upload, mesh, wait) | ~3ms |
| cpu sides only, the same chunk (`build_sides_dig`, before building the mesh) | ~21ms |
| gpu mesh, 32³ noise chunk (45k faces) | ~9ms, cpu sides ~29ms |
| a 10×18×12 chicken: octree rebuild and flatten on the cpu | ~0.25ms |
| the same chicken meshed on the gpu (32³ grid) | ~1.2ms |
| 9 chickens remeshed every frame | ~130 a second at ~14fps (frame bound: llvmpipe drawing) |
| 36 chickens remeshed every frame | ~250 a second at ~7fps |

## How it differs from the cpu path

The gpu mesh is exactly voxel by voxel. The cpu path differs, so meshes won't match face for face yet:

- **Chunk borders:** the cpu culls border faces when there's no neighbour chunk (except the top, `build_voxel_sides`),
  and reads the neighbour's voxels when there is one. The gpu treats the edge as air, it has no neighbours yet.
- **Big nodes:** the cpu decides a big node's faces from neighbour nodes' values (`adjacent_node->value`, the mode),
  so a face partly next to air can be culled. The gpu checks every voxel.
- **Merging:** the cpu draws one quad per big node face, the gpu one per voxel face: more vertices for solid areas.
- **Not done:** textures (terrain uses the tilemap's uvs), lights, ambient occlusion, transparent blocks (the
  `solids` table), block voxes.

## Plan for the game

Each step behind `zox_chunks3_gpu_meshing` and `zox_gpu_compute_supported()`, the cpu path unchanged, a test first.

1. **Neighbours.** Updates stay per chunk. Give the shader its 6 neighbours: upload them flattened in the same buffer
   (an offset per neighbour as a uniform), `voxel_at` steps into the neighbour past the edge. Match the cpu's border
   rules (missing neighbour: cull, top: draw). Test: the existing `chunk_sides` cases, gpu against cpu, face for face.
2. **Batching.** Many dirty chunks in one dispatch: one big nodes buffer with an offset per chunk, `gl_WorkGroupID.z`
   or a uniform array picking the chunk, one command per chunk (`glMultiDrawElementsIndirect` on GL, a loop of
   indirect draws on ES). "Per terrain" would be this, the meshes stay per chunk.
3. **Memory.** `cells × 3` faces of `vec4` vertices is ~6MB a 32³ chunk, too much for a terrain. Either pack a vertex
   into one `uint` (5+5+5 bits position, 3 direction, 8 voxel, the rest for a corner index) and build corners in the
   vertex shader, or count first then allocate (a count pass, a prefix sum, a write pass) into one shared buffer.
4. **Textures.** Pass the tilemap's uvs as a uniform or buffer, the vertex shader picks them by voxel and direction,
   drawn with the terrain's textured shader.
5. **LODs and the swap.** Mesh at the chunk's `RenderDepth` (the depth uniform). The mesh is ready the frame after
   its dispatch; the swap (doc/tec/streaming.md) can wait on a `glFenceSync` instead of `MeshBuilt`.
6. **Lights.** Upload the light octree (`LightNode`) flattened the same way, the vertex shader samples it per
   corner as `build_smooth_lights_system` does. Later, generate light on the gpu too.
7. **Merging and AO** once the rest matches.

Then animation: `gpu_mesh_views` shows rebuilding a model's octree on the cpu each frame is cheap (~0.25ms for a
chicken) and the gpu mesh is the heavier part. Animated models could also skip the octree: a dense 3D texture or a
voxel buffer written by a compute "animation" pass.

## Notes

- Raycasts, physics and collisions read the octrees, not meshes, so they don't care which path meshed a chunk.
- The compute and draw shaders are strings in `fun/gpu_mesh.c`: the shader loader adds `#version 300 es`, compute
  needs `310 es`. Move them to `res/shaders` once the loader takes a version per file.
- `zox_gpu_storage_read` maps and waits for the gpu: for tests and counters only, never per frame in the game.
