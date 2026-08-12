#ifndef zoxm_touch
#define zoxm_touch

// NOTE: We can use this to debug touch mode on PC
byte zox_dbg_touch_with_mouse = 0;
#include "set/_.c"
zox_tag(JoystickUI);
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Touch) {
    zoxd_tag(JoystickUI);
    define_systems_touch(world);
    add_hook_spawn_prefabs(spawn_prefabs_touch);
} zox_end_module(Touch);

#endif
