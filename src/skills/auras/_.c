#ifndef zoxm_auras
#define zoxm_auras

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Auras) {
    zox_define_components_auras(world);
    zox_define_systems_auras(world);
    add_hook_spawn_prefabs(spawn_prefabs_auras);
} zox_end_module(Auras);

#endif
