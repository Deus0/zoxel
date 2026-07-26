

void set_skybox_colors(ecs *world, color_rgb top_color, color_rgb bottom_color) {
    if (!skybox) {
        return;
    }
    if (override_sky) {
        top_color = override_sky_fill;
        bottom_color = override_sky_fill;
    }
    zox_set(skybox, ColorRGB, { top_color });
    zox_set(skybox, SecondaryColorRGB, { bottom_color });
    zox_geter_value(skybox, MaterialGPULink, uint, material);
    set_skybox_material_color(material, top_color, bottom_color);
}