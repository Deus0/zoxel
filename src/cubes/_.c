#ifndef zoxm_cubes
#define zoxm_cubes

zox_tag(Cube);
#include "dat/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "dbg/_.c"

zox_begin_module(Cubes) {
    zoxd_tag(Cube);
    add_hook_spawn_prefabs(spawn_prefabs_cubes);
} zox_end_module(Cubes);

#endif
