// the sky between its from and target colours by SkyBlend (tweened 0 to 1, blend_sky_to), the fog and game_sky_color
//  with it: walking into another biome it fades (it snapped)
static inline byte sky_blend_channel(byte from, byte to, float blend) {
    return (byte) (from + (to - from) * blend + 0.5f);
}

void sky_blend_system(iter* it) {
    zox_sys_on_begin();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SkyBlend);
    zox_sys_in(SkyColorFrom);
    zox_sys_in(SkyColorTarget);
    zox_sys_in(ColorRGB);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SkyBlend, blend);
        zox_sys_i(SkyColorFrom, from);
        zox_sys_i(SkyColorTarget, to);
        zox_sys_i(ColorRGB, now);
        const float t = clampf(blend->value, 0, 1);
        const color_rgb sky = {
            sky_blend_channel(from->value.r, to->value.r, t),
            sky_blend_channel(from->value.g, to->value.g, t),
            sky_blend_channel(from->value.b, to->value.b, t)
        };
        if (sky.r == now->value.r && sky.g == now->value.g && sky.b == now->value.b) {
            continue;
        }
        apply_skybox_colors(world, e, sky, sky);
        game_sky_color = sky;
        game_sky_bottom_color = sky;
        set_camera_fog_color(world, sky);
    }
    zox_sys_on_end();
} zoxd_system(sky_blend_system);
