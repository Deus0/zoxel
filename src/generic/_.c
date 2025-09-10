#ifndef zoxm_generic
#define zoxm_generic

#include "com/_.c"
#include "dat/_.c"
#include "mcr/names.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"

void dispose_generic(ecs* world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_component_ids();
}

zox_begin_module(Generic)
    initialize_component_ids();
    zox_module_dispose(dispose_generic);
    add_hook_spawn_prefabs(spawn_prefabs_generic);
    define_components_generic(world);
    define_systems_generic(world);
zox_end_module(Generic)

#endif