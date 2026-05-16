#ifndef zox_ui_containers
#define zox_ui_containers

#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(UIContainers)
    define_components_ui_containers(world);
    define_systems_ui_containers(world);
zox_end_module(UIContainers)

#endif
