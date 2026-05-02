
zox_sys2(BiomesRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Seed);
    zox_sys_out(BiomeLinks);
    zox_sys_out(Colors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Seed, seed);
        zox_sys_o(BiomeLinks, biomes);
        zox_sys_o(Colors, colors);

        if (state->value != zox_generate_realm_biomes) {
            continue;
        }

        {
            entity grasslands = spawn_biome(world, prefab_biome, e, "grasslands", seed->value + 1);
            add_to_BiomeLinks(biomes, grasslands);
        }

        {
            entity desert = spawn_biome(world, prefab_biome, e, "desert", seed->value + 2);
            add_to_BiomeLinks(biomes, desert);
        }

        zox_logv("At [%f] Realm [Biomes] [%i] spawned.", zox_current_time, biomes->length);

        resize_Colors(colors, realm_colors_count);
        generate_colors(seed->value, colors);

        if (grayscale_mode) {
            for (int j = 0; j < colors->length; j++) {
                colors->value[j] = color_grayscale(colors->value[j].r);
            }
        }

        // rendering: set global game colors
        // fog_color = sky_color;
        color_rgb sky_color = color_to_color_rgb(colors->value[0]);
        game_sky_color = sky_color;
        game_sky_bottom_color = sky_color;
        // viewport_clear_color = sky_color;
        set_camera_fog_color(world, sky_color);

        zox_logv("At [%f] Realm [Colors] [%i] spawned.", zox_current_time, colors->length);
    }
} zox_sys_end(BiomesRealmSpawnSystem);
