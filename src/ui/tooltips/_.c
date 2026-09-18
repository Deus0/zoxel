#ifndef zoxm_tooltips
#define zoxm_tooltips

#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

zox_begin_module(Tooltips) {
    zox_components_tooltips(world);
    zox_systems_tooltips(world);
} zox_end_module(Tooltips);

#endif
