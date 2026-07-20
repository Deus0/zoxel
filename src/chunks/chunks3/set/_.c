#include "color.c"
#include "raycast.c"
#include "scale.c"
#include "lod.c"
byte character_depth = 5;
byte block_vox_depth = 4;
byte2 block_vox_depth_limits = (byte2) { 1, 5 };
byte terrain_depth = 4;
byte2 terrain_depth_limits = (byte2) { 2, 5 };
int fill_octree_random_rate = 50;
int fill_octree_random_rate2 = 40;
int fill_octree_random_rate3 = 30;
