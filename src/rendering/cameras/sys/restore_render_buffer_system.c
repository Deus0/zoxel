// restore render buffer for camera
zox_sys2(RenderBufferRestoreSystem) {
    zox_sys_begin();
    zox_sys_in(ScreenDimensions);
    zox_sys_out(FrameBufferLink);
    zox_sys_out(RenderBufferLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(ScreenDimensions, size);
        zox_sys_o(FrameBufferLink, fbo);
        zox_sys_o(RenderBufferLink, rbo);
        fbo->value = zox_gpu_create_fbo();
        rbo->value = gpu_spawn_render_buffer(size->value);
        if (fbo->value && rbo->value) {
            zox_gpu_link_fbo_rbo(fbo->value, rbo->value);
        }
    }
} zox_sys_end(RenderBufferRestoreSystem);