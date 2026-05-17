#ifndef zox_ui_containers
#define zox_ui_containers

#include "com/_.c"
#include "dat/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(UIContainers)
    zox_define_components_ui_containers(world);
    zox_define_systems_ui_containers(world);
    add_hook_spawn_prefabs(zox_spawn_prefabs_ui_containers);
zox_end_module(UIContainers)

#endif
