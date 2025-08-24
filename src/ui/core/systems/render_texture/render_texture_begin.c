void RenderTextureBeginSystem(iter *it) {

    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeElement);
    zox_sys_in(LayoutSize);
    zox_sys_in(CameraLink);
    zox_sys_in(TextureGPULink);

    for (int i = 0; i < it->count; i++) {

        zox_sys_e();
        zox_sys_i(InitializeElement, initialize);
        zox_sys_i(LayoutSize, size);
        zox_sys_i(CameraLink, camera);
        zox_sys_i(TextureGPULink, gpu_link);

        if (initialize->value != zox_dirty_active) {
            continue;
        }

        if (!gpu_link->value) {
            zox_log_error("render_texture failed: no texture gpu link")
            continue;
        }

        if (!zox_valid(camera->value) || !zox_has(camera->value, FrameBufferLink)) {
            zox_log_error("render_texture failed: camera_invalid [%s]", zox_get_name(camera->value))
            continue;
        }

        if (!zox_has(camera->value, FrameBufferLink)) {
            continue;
        }

        // one time only
        zox_geter_value(camera->value, FrameBufferLink, uint, fbo);
        connect_render_texture_to_fbo(fbo, gpu_link->value);

        // set size
        const int2 scaled_size = scale_viewport(size->value);
        zox_geter_value(camera->value, RenderBufferLink, uint, rbo);
        set_render_texture_gpu(gpu_link->value, scaled_size);
        set_render_buffer_size(rbo, scaled_size);

        /*zox_log("+ [%s] RenderTexture Uploaded: %ix%i - og [%ix%i]",
            zox_get_name(e),
            scaled_size.x, scaled_size.y,
            size->value.x, size->value.y);*/

    }
} zoxd_system(RenderTextureBeginSystem)