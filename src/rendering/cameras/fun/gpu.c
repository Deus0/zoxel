entity_array_d* disposed_render_cameras = NULL;

// lets go of the render cameras' frame and render buffers
void gpu_dispose_render_cameras(ecs* world) {
    zox_each_begin(FrameBufferLink);
        zox_each_o(FrameBufferLink, fbo);
        if (!fbo->value) continue;
        zox_gpu_dispose_fbo(fbo->value);
        fbo->value = 0;
        zox_disposed_add(&disposed_render_cameras, e);
    zox_each_end();
    zox_each_begin(RenderBufferLink);
        zox_each_o(RenderBufferLink, rbo);
        zox_gpu_dispose_rbo(rbo->value);
        rbo->value = 0;
    zox_each_end();
}

// makes a render camera's buffers again, its render textures draw to them again (RenderTextureDirty)
void gpu_restore_render_cameras(ecs* world) {
    zox_disposed_begin(disposed_render_cameras, FrameBufferLink, fbo);
        const PixelSize* size = zox_get(e, PixelSize);
        RenderBufferLink* rbo = zox_get_mut(e, RenderBufferLink);
        if (!size || !rbo) continue;
        fbo->value = zox_gpu_create_fbo();
        rbo->value = gpu_spawn_render_buffer(size->value);
        if (fbo->value && rbo->value) {
            zox_gpu_link_fbo_rbo(fbo->value, rbo->value);
        } else {
            zox_loge("restoring [%s]: no fbo or rbo", zox_getn(e));
        }
    zox_disposed_end(disposed_render_cameras);
    // ones with a texture: one still to make gets its own RenderTextureDirty
    entity_array_d* render_textures = NULL;
    zox_each_begin(RenderTexture);
        if (zox_has(e, TextureGPULink) && zox_getv(e, TextureGPULink)) zox_disposed_add(&render_textures, e);
    zox_each_end();
    zox_disposed_begin(render_textures, TextureGPULink, texture);
        zox_add(e, RenderTextureDirty);
    zox_disposed_end(render_textures);
}
