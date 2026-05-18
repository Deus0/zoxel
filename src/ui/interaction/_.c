#ifndef zox_elements_interaction
#define zox_elements_interaction

#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Interaction) {
    zox_define_components_interaction(world);
    zox_define_systems_interaction(world);
    add_hook_spawn_prefabs(zox_spawn_prefabs_elements_interaction);
} zox_end_module(Interaction)

#endif
