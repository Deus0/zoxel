// this is used for visually representing data
#ifndef zoxm_plots
#define zoxm_plots

zox_tag(Plot);
zox_tag(PlotLine);
zox_tag(PlotLabel);
zoxc_byte(PlotPaused);
zoxc_double(PlotMin);
zoxc_double(PlotMax);
#include "pre/_.c"
#include "sys/_.c"
#include "fun/time.c"

zox_begin_module(Plots) {
    add_hook_spawn_prefabs(spawn_prefabs_plots);
    zoxd_tag(Plot);
    zoxd_tag(PlotLine);
    zoxd_tag(PlotLabel);
    zoxd_byte(PlotPaused);
    zoxd_double(PlotMin);
    zoxd_double(PlotMax);
    define_systems_plots(world);
} zox_end_module(Plots);

#endif
