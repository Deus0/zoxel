#ifndef zox_rendering_cameras
#define zox_rendering_cameras

#ifdef zox_android
    byte zox_disable_post_processing = 1;
#else
    byte zox_disable_post_processing = 0;
#endif

#include "com/_.c"
#include "fun/render_camera.c"
#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(RenderingCameras) {
    zoxd_comopnents_render_cameras(world);
    define_systems_rendering_cameras(world);
    add_hook_spawn_prefabs(spawn_prefabs_rendering_cameras);
} zox_end_module(RenderingCameras);

#endif
