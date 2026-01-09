#ifndef zoxm_touch
#define zoxm_touch

#include "set/_.c"
zox_tag(JoystickUI);
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Touch)
    zoxd_tag(JoystickUI);
    add_hook_spawn_prefabs(spawn_prefabs_touch);
    define_systems_touch(world);
zox_end_module(Touch)

#endif