/*
 * Bodys are Combining Voxes
 *
 *      - Items
 *      - Body UI
 *
 *      Used By:
 *          - Characters
 *
*/
#ifndef zoxm_bodys
#define zoxm_bodys

/*#include "set/_.c"
#include "pre/_.c"
#include "ins/_.c"*/
#include "com/_.c"
#include "sys/_.c"

zox_begin_module(Bodys)
    define_components_bodys(world);
    define_systems_bodys(world);

    /*
    add_hook_spawn_prefabs(spawn_prefabs_models);
    initialize_settings_models(world);*/
zox_end_module(Bodys)

#endif
