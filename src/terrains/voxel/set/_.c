// hmm
byte zox_log_terrain_generation = 0;
// debug
byte zox_dbg_boost_generation_depth = 0;
byte zox_dbg_disable_generation_lods = 0;
// voxes lods
const byte max_vox_blocks = 16; // max spawned vox_blocks
const byte max_lod_voxes = 0;
const float end_game_delay = 0.8f;
const float end_game_delay2 = 1.0f;
const float end_game_delay_fade = 1;
byte high_resolution_terain_lod = 1; // 2 | 1
// byte terrain_spawn_distance;
byte terrain_vertical = 2;
byte terrain_size = 0;
// uint terrain_seed = 32666;
#define terrain_texture_resolution 16 // 32
#define octree_min_height -1.995f // 0.005f
const int2 chunk_texture_size = { terrain_texture_resolution, terrain_texture_resolution };
#include "grassy_plains.c"
#include "resolution.c"
