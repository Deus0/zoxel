// a render camera's colour target is half floats only while a tonemap or bloom is on (doc/plans/hdr.md): clamp drops
//  what's past 1, so bytes there (half the memory)
static inline byte zox_camera_hdr() {
    const byte output = zox_hdr_output == zox_hdr_output_hdr10 || zox_hdr_output == zox_hdr_output_scrgb;
    return (zox_tonemap || zox_bloom || output) && zox_gpu_hdr_supported();
}

// bloom: the target's mips, made after each draw, blurred into the view by its pass
static inline byte zox_camera_bloom() {
    return zox_bloom && zox_camera_hdr() && zox_gpu_bloom_supported();
}

// every render texture made again (its format or mips changed)
static void zox_remake_render_textures(ecs* world) {
    entity_array_d* render_textures = NULL;
    zox_each_begin(RenderTexture);
        if (zox_has(e, TextureGPULink) && zox_getv(e, TextureGPULink)) zox_disposed_add(&render_textures, e);
    zox_each_end();
    zox_disposed_begin(render_textures, TextureGPULink, texture);
        zox_add(e, RenderTextureDirty);
    zox_disposed_end(render_textures);
}

// a post setting changed: its render textures made again when their format or mips change
static void zox_set_post_setting(ecs* world, byte* setting, byte value) {
    const byte was_hdr = zox_camera_hdr(), was_bloom = zox_camera_bloom();
    *setting = value;
    if (zox_camera_hdr() != was_hdr || zox_camera_bloom() != was_bloom) {
        zox_remake_render_textures(world);
    }
}

void zox_set_tonemap(ecs* world, byte value) {
    zox_set_post_setting(world, &zox_tonemap, value);
}

void zox_set_bloom(ecs* world, byte value) {
    zox_set_post_setting(world, &zox_bloom, value);
}

// a camera's colour target: half floats while a tonemap or bloom is on (colours past 1, the post pass tonemaps and
//  blooms them, doc/plans/hdr.md), with mips for bloom, else bytes
static inline void set_render_texture_gpu(guint index, int2 size, byte has_alpha) {
    if (zox_camera_hdr()) {
        zox_gpu_set_texture_color_hdr(index, size, zox_camera_bloom());
    } else if (has_alpha) {
        zox_gpu_set_texture_color_rgba(index, size, NULL);
    } else {
        zox_gpu_set_texture_color_rgb(index, size, NULL);
    }
}
