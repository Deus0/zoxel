#ifndef zoxm_characters3
#define zoxm_characters3

// todo: rename zox_tag to zoxc_tag
byte disable_npc_uis = 0;
byte disable_npc_hooks = 0;
byte disable_npc_positioner = 0;
#include "set/_.c"
#include "com/_.c"
#include "sta/_.c"
#include "dat/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Characters3) {
    zox_components_characters3(world);
    zox_systems_characters3(world);
    set_character_settings();
    add_hook_spawn_prefabs(spawn_prefabs_characters3D);
} zox_end_module(Characters3);

#endif
