#ifndef zox_rendering_cameras
#define zox_rendering_cameras

#include "dat/frame_buffer.c"
#include "dat/render_buffer.c"
#include "fun/render_camera.c"
#include "sys/_.c"

zox_begin_module(RenderingCameras)
    zoxd_dest(RenderBufferLink);
    zoxd_dest(FrameBufferLink);
    define_systems_rendering_cameras(world);
zox_end_module(RenderingCameras)

#endif
