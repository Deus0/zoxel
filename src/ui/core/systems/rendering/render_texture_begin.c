void RenderTextureBeginSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(InitializeElement);
    zox_sys_in(TextureSize);
    zox_sys_in(CameraLink);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(InitializeElement, initializeElement);
        zox_sys_i(TextureSize, textureSize);
        zox_sys_i(CameraLink, cameraLink);
        zox_sys_i(TextureGPULink, gpu_link);
        if (initializeElement->value != zox_dirty_active) {
            continue;
        }

        if (!gpu_link->value) {
            zox_log_error("render_texture failed: no texture gpu link")
            continue;
        }
        if (!zox_valid(cameraLink->value) || !zox_has(cameraLink->value, FrameBufferLink)) {
            zox_log_error("render_texture failed: camera_invalid [%s]", zox_get_name(cameraLink->value))
            continue;
        }

        entity camera = cameraLink->value;
        if (!zox_has(camera, FrameBufferLink)) {
            continue;
        }

        zox_geter_value(camera, FrameBufferLink, uint, fbo);
        zox_geter_value(camera, RenderBufferLink, uint, rbo);
        set_render_texture_gpu(gpu_link->value, textureSize->value);
        connect_render_texture_to_fbo(fbo, gpu_link->value);
        set_render_buffer_size(rbo, textureSize->value);

    }
} zoxd_system(RenderTextureBeginSystem)