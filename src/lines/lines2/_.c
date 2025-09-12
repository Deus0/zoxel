#ifndef zoxm_lines2
#define zoxm_lines2

zox_tag(Line2D);
zox_tag(ElementLine2D);
zoxc(LineData2D, float4);
zoxc(LinePosition2, int4);
zoxc(LineLocalPosition2, int4);
zoxc(LineAnchor, float4);
#include "shd/line2D.c"
#include "fun/line_util.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Lines2D)
    zoxd_tag(Line2D);
    zoxd_tag(ElementLine2D);
    zoxd(LineData2D);
    zoxd(LinePosition2);
    zoxd(LineLocalPosition2);
    zoxd(LineAnchor);
    define_systems_line2D(world);
    add_hook_load_shader(&spawn_shader_line2D);
    add_hook_spawn_prefabs(spawn_prefabs_lines2D);
zox_end_module(Lines2D)

#endif