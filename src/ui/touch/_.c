
// NOTE: We can use this to debug touch mode on PC
byte zox_dbg_touch_with_mouse = 0;
#include "set/_.c"
zox_tag(JoystickUI);
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

void import_touch(ecs* world) {
    zox_module(touch);
    zoxd_tag(JoystickUI);
    zox_systems_touch(world);
    add_hook_spawn_prefabs(spawn_prefabs_touch);
}