#include "frame_buffer.c"
#include "render_buffer.c"
zox_tag(RenderTextureAlpha);

void zoxd_comopnents_render_cameras(ecs* world) {
    zoxd_dest(RenderBufferLink);
    zoxd_dest(FrameBufferLink);
    zoxd_tag(RenderTextureAlpha);
}