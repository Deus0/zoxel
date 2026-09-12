zox_sys2(RenderTextureInitializeSystem) {
    byte dbg_log = 0 || is_verbose;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TextureSize);
    zox_sys_in(TextureGPULink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TextureSize, tsize);
        zox_sys_i(TextureGPULink, texture);
        if (!texture->value) {
            zox_loge("[RenderTextureInitializeSystem] Invalid [TextureGPULink] on [%s]", zox_sys_e_name);
            continue;
        }
        entity camera = zox_get_link(world, e, CameraLink);
        if (!zox_valid(camera)) {
            zox_loge("Invalid Render Camera on [%s]", zox_getn(e));
            continue;
        }
        if (!zox_has(camera, FrameBufferLink)) {
            zox_loge("Render Camera has no FrameBufferLink [%s]", zox_getn(camera));
            continue;
        }
        // one time only
        guint fbo = zox_getv(camera, FrameBufferLink);
        if (!fbo) {
            zox_loge("[RenderTextureInitializeSystem] FBO Invalid on camera [%s]", zox_getn(camera));
            continue;
        }
        guint rbo = zox_getv(camera, RenderBufferLink);
        if (!rbo) {
            zox_loge("[RenderTextureInitializeSystem] RBO Invalid on camera [%s]", zox_getn(camera));
            continue;
        }
        byte is_alpha = zox_has(e, RenderTextureAlpha);
        // now we set it
        set_render_texture_gpu(
            texture->value,
            tsize->value,
            is_alpha);
        zox_gpu_fbo_to_texture(fbo, texture->value);
        // This actually effects our render camera tho
        zox_gpu_set_rbo_size(rbo, tsize->value);
        // Done!
        zox_remove(e, RenderTextureDirty);
        if (dbg_log) {
            zox_log("Initialized Render Texture [%s] [%ix%i] Texture [%u] -> Camera [%s] Fbo [%u] Rbo [%u] - Alpha [%i]",
                zox_sys_e_name,
                tsize->value.x,
                tsize->value.y,
                texture->value,
                zox_getn(camera),
                fbo,
                rbo,
                is_alpha);
        }
    }
} zox_sys_end(RenderTextureInitializeSystem);
