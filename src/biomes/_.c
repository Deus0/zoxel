/**
 *  Biomes:
 *      - Colors
 *      Blocks
 *      Monsters
 *      Dungeons
 *
 *  Sub modules can form unique biome features
 *
 * */

byte zox_disable_biomes = 0;
byte zox_biome_override = 0;    // stick to earthy for now
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

// TODO: Add Biome Blocks to Realm, set VoxelID then
// TODO: Use Biome Blocks in Terrain Generation
// TODO: Fix Sky Color - set when entering biome for camera
//      - BiomeLink and set it for camera system - BiomeInsideSystem

void import_biomes(ecs* world) {
    zox_module(biomes);
    zox_components_biomes(world);
    zox_systems_biomes(world);
    add_hook_spawn_prefabs(spawn_prefabs_biomes);
}