#ifndef zoxm_lines3
#define zoxm_lines3

// should live for .1 seconds
const float line_frame_timing = 1 / 10.0f;
zox_tag(Line3D);
zoxc(LineData3D, float6);
zox_tag(CubeLines);
zoxc_byte(DebugCubeLines);
zoxc_float(CubeLinesThickness);
zox_tag(DebugCubeCorner);
zoxc_float(DebugCubeShrink);
#include "sta/_.c"
#include "pre/_.c"
#include "shd/line3D.c"
#include "fun/_.c"
#include "sys/_.c"

void spawn_shaders_lines3D(ecs_world_t *world) {
    if (render_backend == zox_render_backend_opengl) {
        initialize_shader_line3D(world);
    }
}

zox_begin_module(Lines3)
    zoxd_tag(Line3D);
    zoxd_tag(CubeLines);
    zoxd_tag(DebugCubeCorner);
    zoxd(DebugCubeLines);
    zoxd(LineData3D);
    zoxd_float(CubeLinesThickness);
    zoxd_float(DebugCubeShrink);
    define_systems_lines3(world);
    add_hook_load_shader(&spawn_shaders_lines3D);
    add_hook_spawn_prefabs(spawn_prefabs_lines3D);
zox_end_module(Lines3)

#endif