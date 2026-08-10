zox_sys2(RenderTextureBeginSystem) {
    byte dbg_log = 0 || is_verbose;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Initialize);
    zox_sys_in(TextureSize);
    zox_sys_in(CameraLink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Initialize, initialize);
        zox_sys_i(TextureSize, tsize);
        zox_sys_i(CameraLink, camera);
        zox_sys_i(TextureGPULink, texture);
        if (initialize->value != zox_dirty_end) { // active) {
            continue;
        }
        if (!texture->value) {
            zox_loge("render_texture failed: no texture gpu link");
            continue;
        }
        if (!zox_valid(camera->value)) {
            zox_loge("Invalid Render Camera on [%s]", zox_getn(e));
            continue;
        }
        if (!zox_has(camera->value, FrameBufferLink)) {
            zox_log_error("Render Camera has no FrameBufferLink [%s]", zox_get_name(camera->value));
            continue;
        }
        // one time only
        guint fbo = zox_getv(camera->value, FrameBufferLink);
        if (!fbo) {
            zox_loge("[RenderTextureBeginSystem] FBO Invalid on camera [%s]", zox_getn(camera->value));
            continue;
        }
        guint rbo = zox_getv(camera->value, RenderBufferLink);
        if (!rbo) {
            zox_loge("[RenderTextureBeginSystem] RBO Invalid on camera [%s]", zox_getn(camera->value));
            continue;
        }
        zox_gpu_fbo_to_texture(fbo, texture->value);
        set_render_texture_gpu(texture->value, tsize->value, zox_has(e, RenderTextureAlpha));
        zox_gpu_set_rbo_size(rbo, tsize->value);
        if (dbg_log) {
            zox_log("+ [%s] RenderTexture Uploaded: %ix%i", zox_getn(e), tsize->value.x, tsize->value.y);
        }
    }
} zox_sys_end(RenderTextureBeginSystem);
