#ifndef zoxm_biomes
#define zoxm_biomes
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

byte zox_biome_override = 0;    // stick to earthy for now
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(Biomes)
    add_hook_spawn_prefabs(spawn_prefabs_biomes);
    define_components_biomes(world);
    define_systems_biomes(world);
zox_end_module(Biomes)

#endif
