
// #define zox_debug_billboard_system
float zox_ui_scale3 = 0.05f;
float zox_popup_scale = 0.05f;
#include "com/_.c"
#include "dat/_.c"
#include "set/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_elements3(ecs* world) {
    zox_module(elements3);
    zoxd_components_elemments3(world);
    zox_systems_elements3D(world);
    add_hook_spawn_prefabs(spawn_prefabs_elements3D);
}