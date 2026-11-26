// a render camera's colour target is half floats only while a tonemap is on (doc/plans/hdr.md): clamp drops what's
//  past 1, so bytes there (half the memory)
static inline byte zox_camera_hdr() {
    return zox_tonemap && zox_gpu_hdr_supported();
}

// the Tonemap setting: its render textures made again in the other format when it turns on or off
void zox_set_tonemap(ecs* world, byte value) {
    const byte was_hdr = zox_camera_hdr();
    zox_tonemap = value;
    if (zox_camera_hdr() == was_hdr) {
        return;
    }
    entity_array_d* render_textures = NULL;
    zox_each_begin(RenderTexture);
        if (zox_has(e, TextureGPULink) && zox_getv(e, TextureGPULink)) zox_disposed_add(&render_textures, e);
    zox_each_end();
    zox_disposed_begin(render_textures, TextureGPULink, texture);
        zox_add(e, RenderTextureDirty);
    zox_disposed_end(render_textures);
}

// a camera's colour target: half floats while a tonemap is on (colours past 1, the post pass tonemaps them,
//  doc/plans/hdr.md), else bytes
static inline void set_render_texture_gpu(guint index, int2 size, byte has_alpha) {
    if (zox_camera_hdr()) {
        zox_gpu_set_texture_color_hdr(index, size);
    } else if (has_alpha) {
        zox_gpu_set_texture_color_rgba(index, size, NULL);
    } else {
        zox_gpu_set_texture_color_rgb(index, size, NULL);
    }
}
