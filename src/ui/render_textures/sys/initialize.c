zox_sys2(RenderTextureBeginSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeElement);
    zox_sys_in(TextureSize);
    zox_sys_in(CameraLink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InitializeElement, initialize);
        zox_sys_i(TextureSize, tsize);
        zox_sys_i(CameraLink, camera);
        zox_sys_i(TextureGPULink, gpu_link);

        if (initialize->value != zox_dirty_active) {
            continue;
        }

        if (!gpu_link->value) {
            zox_log_error("render_texture failed: no texture gpu link")
            continue;
        }

        if (!zox_valid(camera->value)) {
            zox_logw("Invalid Render Camera [%lu]", camera->value);
            continue;
        }

        if (!zox_has(camera->value, FrameBufferLink)) {
            zox_log_error("Render Camera has no FrameBufferLink [%s]", zox_get_name(camera->value));
            continue;
        }

        // one time only
        zox_geter_value(camera->value, FrameBufferLink, uint, fbo);
        connect_render_texture_to_fbo(fbo, gpu_link->value);

        // set size
        zox_geter_value(camera->value, RenderBufferLink, uint, rbo);
        set_render_texture_gpu(gpu_link->value, tsize->value);
        set_render_buffer_size(rbo, tsize->value);

        // zox_sys_e();
        // zox_log("+ [%s] RenderTexture Uploaded: %ix%i", zox_get_name(e), tsize->value.x, tsize->value.y);

    }
} zox_sys_end(RenderTextureBeginSystem);
