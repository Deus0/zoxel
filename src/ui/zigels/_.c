#ifndef zoxm_zigels
#define zoxm_zigels

uint font_children_capacity = 512;

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "io/_.c"

zox_begin_module(Zigels)
    add_hook_files_load(load_files_fonts);
    add_hook_spawn_prefabs(spawn_prefabs_zigels);
    define_components_zigels(world);
    define_systems_zigels(world);
zox_end_module(Zigels)

#endif
