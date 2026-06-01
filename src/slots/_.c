#ifndef zoxm_slots
#define zoxm_slots

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"

zox_begin_module(Slots) {
    zox_define_components_slots(world);
    add_hook_spawn_prefabs(zox_spawn_prefabs_slots);
} zox_end_module(Slots)

#endif
