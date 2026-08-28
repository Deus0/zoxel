// NOTE: Restore render buffer for camera
zox_sys2(RenderBufferRestoreSystem) {
    byte dbg_log = 0 || is_log_gpu_restore;
    zox_sys_begin();
    zox_sys_in(PixelSize);
    zox_sys_out(FrameBufferLink);
    zox_sys_out(RenderBufferLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(PixelSize, size);
        zox_sys_o(FrameBufferLink, fbo);
        zox_sys_o(RenderBufferLink, rbo);
        fbo->value = zox_gpu_create_fbo();
        rbo->value = gpu_spawn_render_buffer(size->value);
        if (fbo->value && rbo->value) {
            zox_gpu_link_fbo_rbo(fbo->value, rbo->value);
        } else {
            zox_loge("Issues with fbo/rbos");
        }
        if (dbg_log) {
            zox_log("Restored Render Camera [%s]: FBO [%u] RBO [%u] at [%ix%i]", zox_sys_e_name, fbo->value, rbo->value, size->value.x, size->value.y);
        }
    }
} zox_sys_end(RenderBufferRestoreSystem);


// NOTE: Runs from render textures
zox_sys2(RenderTextureRestoreSystem) {
    byte dbg_log = 0 || is_log_gpu_restore;
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_add(e, RenderTextureDirty);
         if (dbg_log) {
            zox_log("Restored Render Texture [%s]", zox_sys_e_name);
        }
        /*if (!zox_has(camera->value, FrameBufferLink)) {
            zox_loge("camera does not have frame buffer link: %lu", camera->value);
            continue;
        }
        // Refresh Render Camera
        guint fbo = zox_gpu_create_fbo();
        if (!fbo) {
            zox_loge("Failure to make FBO");
            continue;
        }
        zox_setv(camera->value, FrameBufferLink, fbo);
        guint rbo = zox_gpu_create_rbo();
        if (!rbo) {
            zox_loge("Failure to make RBO");
            continue;
        }
        zox_setv(camera->value, RenderBufferLink, rbo);
        zox_gpu_set_rbo_size(rbo, size->value);
        zox_gpu_link_fbo_rbo(fbo, rbo);
        // Refresh Texture
        set_render_texture_gpu(
            texture->value,
            size->value,
            zox_has(e, RenderTextureAlpha));
        zox_gpu_fbo_to_texture(fbo, texture->value);
        zox_gpu_set_rbo_size(rbo, size->value);
        if (dbg_log) {
            zox_log("Restored Render Texture [%s] Texture [%u] -> fbo [%u] and rbo [%u]", zox_sys_e_name, texture->value, fbo, rbo);
        }*/
    }
} zox_sys_end(RenderTextureRestoreSystem);

zox_sys2(FrameBufferDisposeSystem) {
    zox_sys_begin();
    zox_sys_out(FrameBufferLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(FrameBufferLink, fbo);
        zox_gpu_dispose_fbo(fbo->value);
        fbo->value = 0;
    }
} zox_sys_end(FrameBufferDisposeSystem);

zox_sys2(RenderBufferDisposeSystem) {
    zox_sys_begin();
    zox_sys_out(RenderBufferLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_o(RenderBufferLink, rbo);
        zox_gpu_dispose_rbo(rbo->value);
        rbo->value = 0;
    }
} zox_sys_end(RenderBufferDisposeSystem);
