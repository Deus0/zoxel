#ifndef zoxm_ui_core
#define zoxm_ui_core

// todo: move LayoutPosition etc to Transforms/Grid2D - idk what to call it
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(ElementsCore)
    define_systems_elements_core(world);
    add_hook_spawn_prefabs(spawn_prefabs_ui_core);
zox_end_module(ElementsCore)

#endif