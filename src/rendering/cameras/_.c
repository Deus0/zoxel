
#include "com/_.c"
#include "post_processing/_.c"
#include "fun/render_camera.c"
#include "fun/gpu.c"
#include "pre/_.c"
#include "sys/_.c"

void import_render_cameras(ecs* world) {
    zox_module(render_cameras);
    zox_add_module(post_processing);
    zoxd_comopnents_render_cameras(world);
    zox_systems_rendering_cameras(world);
    add_hook_spawn_prefabs(spawn_prefabs_rendering_cameras);
    add_hook_gpu_dispose(gpu_dispose_render_cameras);
    add_hook_gpu_restore(gpu_restore_render_cameras);
}
