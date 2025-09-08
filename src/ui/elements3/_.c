#ifndef zox_elements3
#define zox_elements3

zox_tag(Element3D);
zox_tag(Text3D);
zoxc_float3(UITrail);
zoxc_float(Text3DScale);
#include "dat/_.c"
#include "set/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/popup.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Elements3)
    zoxd_tag(Element3D);
    zoxd_tag(Text3D);
    zoxd(UITrail);
    zoxd_float(Text3DScale);
    define_systems_elements3D(world);
    add_hook_spawn_prefabs(spawn_prefabs_elements3D);
zox_end_module(Elements3)

#endif