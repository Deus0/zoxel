color icon_label_fill = color_white;
color icon_label_outline = color_gray;

#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_elements2(ecs* world) {
    zox_module(elements2);
    zox_components_elements2(world);
    zox_systems_elements2(world);
    add_hook_spawn_prefabs(spawn_prefabs_elements2);
}