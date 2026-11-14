#include "color.c"
#include "raycast.c"
#include "scale.c"
#include "lod.c"
// issue is the character depth is set by blockvox depth atm
byte character_depth = 5;
byte block_depth = 4;
byte2 block_depth_limits = (byte2) { 1, 5 };
byte terrain_depth = 4;
byte2 terrain_depth_limits = (byte2) { 2, 5 };
int fill_octree_random_rate = 50;
int fill_octree_random_rate2 = 40;
int fill_octree_random_rate3 = 30;
// gpu meshing (doc/tec/gpu_meshing.md): chunks meshed by a compute shader from their octree (fun/gpu_mesh.c),
//  cpu meshing (sides, build_textured) stays as the fallback, also when the context has no compute (ES 3.1+)
//  - not read by the chunk systems yet, only the proof of concept exists (tst gpu_mesh, gpu_mesh_views)
byte zox_chunks3_gpu_meshing = 0;
