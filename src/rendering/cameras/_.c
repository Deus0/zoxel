
#include "com/_.c"
#include "fun/render_camera.c"
#include "pre/_.c"
#include "sys/_.c"

void import_render_cameras(ecs* world) {
    zox_module(render_cameras);
    zoxd_comopnents_render_cameras(world);
    define_systems_rendering_cameras(world);
    add_hook_spawn_prefabs(spawn_prefabs_rendering_cameras);
}
