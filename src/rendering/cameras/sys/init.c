zox_sys2(RenderCameraInitializeSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_in(Initialize);
    zox_sys_in(ScreenDimensions);
    zox_sys_out(FrameBufferLink);
    zox_sys_out(RenderBufferLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(Initialize, initialize);
        zox_sys_i(ScreenDimensions, size);
        zox_sys_o(FrameBufferLink, fbo);
        zox_sys_o(RenderBufferLink, rbo);
        /*if (initialize->value != zox_dirty_active) {
            continue;
        }*/
        fbo->value = zox_gpu_create_fbo();
        if (!fbo->value) {
            zox_loge("Failed to create FBO on Render Camera [%s]", zox_getn(e));
            continue;
        }
        rbo->value = zox_gpu_create_rbo();
        if (!rbo) {
            zox_loge("Failed to create RBO on Render Camera [%s]", zox_getn(e));
            continue;
        }
        zox_gpu_set_rbo_size(rbo->value, size->value);
        zox_gpu_link_fbo_rbo(fbo->value, rbo->value);
        if (dbg_log) {
            zox_log("+ [%s] Render Camera Initialized [%ix%i]", zox_getn(e), size->value.x, size->value.y);
        }
    }
} zox_sys_end(RenderCameraInitializeSystem);
