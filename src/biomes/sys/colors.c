// TODO: On Generate Biome - Spawn Blocks
zox_sys2(BiomeColorsSystem) {
    /*zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Seed);
    zox_sys_out(Colors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Seed, seed);
        zox_sys_o(Colors, colors);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        zox_sys_e();
        zox_logv("Generating Colors for biome [%s]", zox_get_name(e));
        color_rgb sky_color;
        if (!grayscale_mode) {
            resize_Colors(colors, realm_colors_count);
            generate_colors(seed->value, (colors));
            sky_color = color_to_color_rgb(colors->value[0]);
        } else {
            sky_color = color_rgb_grayscale(3);
        }
        // Set sky color hhere
        game_sky_color = sky_color;
        game_sky_bottom_color = sky_color;
        set_camera_fog_color(world, sky_color);
    }*/
} zox_sys_end(BiomeColorsSystem);
