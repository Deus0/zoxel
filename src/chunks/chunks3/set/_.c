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
// gpu meshing (doc/plans/gpu_meshing.md): solid chunk meshes meshed by a compute shader (fun/gpu_mesh.c,
//  lights3/sys/gpu_mesh.c), cpu meshing (sides, build_textured) stays as the fallback, also when the context has no
//  compute (ES 3.1+): its meshes go back to the cpu; off by default, on with --gpu_meshing
byte zox_chunks3_gpu_meshing = 0;
