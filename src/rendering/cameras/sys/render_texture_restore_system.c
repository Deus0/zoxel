// NOTE: Runs from render texture itself
zox_sys2(RenderTextureRestoreSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureGPULink);
    zox_sys_in(TextureSize);
    zox_sys_in(CameraLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureGPULink, texture);
        zox_sys_i(TextureSize, size);
        zox_sys_i(CameraLink, camera);
        if (!zox_has(camera->value, FrameBufferLink)) {
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
        set_render_texture_gpu(texture->value, size->value, zox_has(e, RenderTextureAlpha));
        zox_gpu_fbo_to_texture(fbo, texture->value);
        zox_gpu_set_rbo_size(rbo, size->value);
        if (is_log_gpu_restore) {
            zox_sys_world()
            zox_sys_e()
            zox_log("+ restoring render_texture [%s]\n", zox_get_name(e))
            zox_log("   - texture [%i] linked to fbo [%i] and rbo [%i]", texture->value, fbo, rbo)
        }
    }
} zox_sys_end(RenderTextureRestoreSystem);
