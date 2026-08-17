

void set_skybox_colors(ecs *world, color_rgb top_color, color_rgb bottom_color) {
    if (!zox_valid(skybox)) {
        zox_loge("Skybox Invalid");
        return;
    }
    if (override_sky) {
        top_color = override_sky_fill;
        bottom_color = override_sky_fill;
    }
    zox_setv(skybox, ColorRGB, top_color);
    zox_setv(skybox, SecondaryColorRGB, bottom_color);
    guint material = zox_getv(skybox, MaterialGPULink);
    set_skybox_material_color(material, top_color, bottom_color);
}