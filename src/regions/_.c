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
#define zox_generate_region_start 1
#define zox_generate_region_mountains 1
#define zox_generate_region_towns_trigger 2
#define zox_generate_region_towns 3
#define zox_generate_region_towns_homes 4
#define zox_generate_region_dungeons 5
#define zox_generate_region_end 0

byte region_dividor = 32;
// Regions
// TODO: Recalculate all the RegionLods when Dirty
// NOTE: Kickstart System exists in terrain atm, move those to Tunk/Regions
// Region Sub Spawns:
// TODO: Spawn Towns when region generates

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_regions(ecs* world) {
    zox_module(regions);
    zox_components_regions(world);
    zox_systems_regions(world);
    add_hook_spawn_prefabs(spawn_prefabs_regions);
}
