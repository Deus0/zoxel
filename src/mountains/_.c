/*
 *  Zox Regions
 *      - A World has Many Regions
 *      - Made up of Tunks and Chunks
 *
 *  Regions Have
 *      - Towns
 *      - Highways
 *      - Mountains
 *      - Dungeons
 *
 * */
/*
 *  Zox Mountains
 *
 *      - Big Hills
 *      - Waterfalls?
 *      - Uses Regions, Tunks, Chunks to exist
 *
 * */
byte zox_disable_mountains = 0;
#define zox_mountain_type_hill 0
#define zox_mountain_type_peak 1
#define zox_mountain_type_end 2
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

void import_mountains(ecs* world) {
    zox_module(mountains);
    zox_define_components_mountains(world);
    zox_define_systems_mountains(world);
    add_hook_spawn_prefabs(zox_define_prefabs_mountains);
}
