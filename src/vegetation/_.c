/**
 *  Zox Vegetation
 *
 *      - Spawns Fauna Blocks in Biomes
 *      - Places in Tunks
 *      - Places in Chunks
 *
 * */
byte zox_disable_vegetation = 0;
#include "com/_.c"
#include "pre/_.c"
#include "sys/_.c"

void import_vegetation(ecs* world) {
    zox_module(vegetation);
    zox_components_vegetation(world);
    zox_systems_vegetation(world);
    add_hook_spawn_prefabs(spawn_prefabs_vegetation);
    // add_hook_spawn_prefabs(spawn_prefabs_tunks);
    // zox_systems_tunks_debug(world);
}
