// this is used for visually representing data
#ifndef zoxm_plots
#define zoxm_plots

#include "com/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/time.c"

zox_begin_module(Plots) {
    add_hook_spawn_prefabs(spawn_prefabs_plots);
    zox_define_components_plots(world);
    define_systems_plots(world);
} zox_end_module(Plots);

#endif
