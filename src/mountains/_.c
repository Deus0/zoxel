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
#ifndef zox_mountains
#define zox_mountains

byte zox_disable_mountains = 0;
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Mountains) {
    zox_define_components_mountains(world);
    zox_define_systems_mountains(world);
    add_hook_spawn_prefabs(zox_define_prefabs_mountains);
} zox_end_module(Mountains);

#endif
