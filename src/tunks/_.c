/**
 *  Tunks - Chunk2s
 *
 *
 *      - Heightmaps
 *      - Biomes
 *      - Precipation
 *      - Temperature
 *      - TownMaps
 *
 * */
byte zox_flatlands = 0;

// Tunk States
#define zox_generate_tunk_start 1
#define zox_generate_tunk_biomes 1
#define zox_generate_tunk_heights 2
#define zox_generate_tunk_vegetation 3
#define zox_generate_tunk_mountains 4
#define zox_generate_tunk_towns 5
#define zox_generate_tunk_end 6

#define zox_vegetation_dirt 0
#define zox_vegetation_grass 1
#define zox_vegetation_weeds 2
#define zox_vegetation_trees 3
#define zox_vegetation_flowers 4

// TODO: Vegetation Map for Grass  / trees etc
// TODO: Maps Update based on LODs??
// TODO: Quadtrees for map data?
// TODO: After BiomeMap is generated, get most used byte, then link

// TODO: Calculate biome highest based on N biomes, get max from realm

// TODO: Use sbyte instead of byte?
// TODO: Chunk2NeighborsSystem

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_tunks(ecs* world) {
    zox_module(tunks);
    zox_components_tunks(world);
    zox_systems_tunks(world);
    add_hook_spawn_prefabs(spawn_prefabs_tunks);
    zox_systems_tunks_debug(world);
}
