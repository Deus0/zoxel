#include "mesh.c"
#include "terrain_place.c"

// #define DISABLE_AO
#define AO_MULTIPLIER 1.4f // 2
#define ao_no_neighbors 1
#define ao_small_darkness 0.8f
#define ao_edge_darkness 0.6f
#define ao_corner_darkness 0.25f


// Chunk States
#define zox_generate_terrain_start 1
#define zox_generate_terrain_landfill 1
#define zox_generate_terrain_vegetation 2
#define zox_generate_terrain_towns 3
#define zox_generate_terrain_sunlight 4
#define zox_generate_terrain_end2 5

byte zox_dbg_disable_chunk_mesh = 0;
byte zox_dbg_npc_all_max_depth = 0;
byte zox_split_textured_quads = 1;
byte zox_dbg_render_all_sides = 0;
byte zox_split_colored_chunks = 0;
byte zox_ambient_occlusion = 1;
#define zox_ambient_occlusion27

