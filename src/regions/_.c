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
#ifndef zox_regions
#define zox_regions

// Regions
// TODO: RegionStreamerDirty Detection
// TODO: Recalculate all the RegionLods when Dirty
// NOTE: Kickstart System exists in terrain atm, move those to Tunk/Regions
// Region Sub Spawns:
// TODO: Spawn Towns when region generates

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Regions)
    zox_define_components_regions(world);
    zox_define_systems_regions(world);
    add_hook_spawn_prefabs(spawn_prefabs_regions);
zox_end_module(Players)

#endif
