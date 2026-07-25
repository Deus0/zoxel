/**
 *  Zox Vegetation
 *
 *      - Spawns Fauna Blocks in Biomes
 *      - Places in Tunks
 *      - Places in Chunks
 *
 * */
#ifndef zoxm_vegetation
#define zoxm_vegetation

byte zox_disable_vegetation = 0;
#include "com/_.c"
#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(Vegetation) {
    zox_define_components_vegetation(world);
    zox_define_systems_vegetation(world);
    add_hook_spawn_prefabs(spawn_prefabs_vegetation);
    // add_hook_spawn_prefabs(spawn_prefabs_tunks);
    // zox_define_systems_tunks_debug(world);
} zox_end_module(Vegetation);

#endif
