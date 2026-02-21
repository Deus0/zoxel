extern double terrain_frequency;

byte realm_colors_count = 8;

void generate_colors(lint seed, Colors *colors) {

    srand((unsigned int) seed);

    // byte biome_type = rand() % zox_biome_end;
    BiomeData biome = pick_biome(seed);
    terrain_frequency = biome.frequency;

    float3 dirt_hsv = float6_rand_range(biome.dirt);
    float3 grass_hsv = float6_rand_range(biome.grass);
    float3 sand_hsv = float6_rand_range(biome.sand);
    float3 stone_hsv = float6_rand_range(biome.sand);
    float3 obsidian_hsv = float6_rand_range(biome.obsidian);
    float3 sky_hsv = float6_rand_range(biome.sky);
    float3 wood_hsv = (float3) {
            dirt_hsv.x + rand() % 30,
            dirt_hsv.y - 10,
            dirt_hsv.z + 10 + rand() % 10 };


    color dirt_color = hsv_to_color(dirt_hsv);
    color grass_color = hsv_to_color(grass_hsv);
    color sand_color = hsv_to_color(sand_hsv);
    color stone_color = hsv_to_color(stone_hsv);
    color sky_color = hsv_to_color(sky_hsv);
    color obsidian_color = hsv_to_color(obsidian_hsv);
    color woodc = hsv_to_color(wood_hsv);

    byte i = 0;
    colors->value[i++] = sky_color;
    colors->value[i++] = dirt_color;
    colors->value[i++] = grass_color;
    colors->value[i++] = sand_color;
    colors->value[i++] = stone_color;
    colors->value[i++] = obsidian_color;
    colors->value[i++] = woodc;

    zox_logv("Biome: %s", biome.name);
    zox_logv("  Dirt: %fx%fx%f", dirt_hsv.x, dirt_hsv.y, dirt_hsv.z);
    zox_logv("  Grass: %fx%fx%f", grass_hsv.x, grass_hsv.y, grass_hsv.z);
    zox_logv("  Sand: %fx%fx%f", sand_hsv.x, sand_hsv.y, sand_hsv.z);
    zox_logv("  Stone: %fx%fx%f", sand_hsv.x, sand_hsv.y, sand_hsv.z);
    zox_logv("  Obsidian: %fx%fx%f", obsidian_hsv.x, obsidian_hsv.y, obsidian_hsv.z);
    zox_logv("  Sky: %fx%fx%f", sky_hsv.x, sky_hsv.y, sky_hsv.z);
}

extern void set_camera_fog_color(ecs*, color_rgb) ;

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


    /*const float6 hsvr_dirt = get_dirt_hsv_range(biome_type);
    float3 dirt_hsv = generate_hsv_v_s(
        (float2) { hsvr_dirt.x, hsvr_dirt.y },
        (float2) { hsvr_dirt.z, hsvr_dirt.w },
        (float2) { hsvr_dirt.u, hsvr_dirt.v });

    const float6 hsvr_grass = get_grass_hsv_range(biome_type);
    float3 grass_hsv = generate_hsv_v_s(
        (float2) { hsvr_grass.x, hsvr_grass.y },
        (float2) { hsvr_grass.z, hsvr_grass.w },
        (float2) { hsvr_grass.u, hsvr_grass.v });

    const float6 hsvr_sand = get_sand_hsv_range(biome_type);
    float3 sand_hsv = generate_hsv_v_s(
        (float2) { hsvr_sand.x, hsvr_sand.y },
        (float2) { hsvr_sand.z, hsvr_sand.w },
        (float2) { hsvr_sand.u, hsvr_sand.v });*/
    /*const float2 stone_hue = (float2) { 0, 360 };
    const float2 stone_value = (float2) { 40, 60 };
    const float2 stone_saturation = (float2) { 2, 6 };
    const float3 stone_hsv = generate_hsv_v_s(stone_hue, stone_value, stone_saturation);*/

    /*float3 obsidian_hsv = (float3) {
        rand() % 360,
        8 + rand() % 8,
        13
    };
    if (obsidian_hsv.x + 180 >= dirt_hsv.x + 180 - 45 && obsidian_hsv.x + 180 <= dirt_hsv.x + 180 + 45) {
        obsidian_hsv.x += 180; // if close to dirt, move away the hue
    }
    const float2 sky_hue = (float2) { 0, 360 }; // ((int) grass_hsv.x + 180) % 360)
    const float2 sky_value = (float2) { 54, 66 };
    const float2 sky_saturation = (float2) { 16, 30 };
    float3 sky_hsv = generate_hsv_v_s(sky_hue, sky_value, sky_saturation);*/
    // float3 sky_hsv = (float3) { (float) (((int) grass_hsv.x + 180) % 360), dirt_hsv.y + 32,
        // dirt_hsv.z + 32 };



    // dirt color - hsv - hue saturation value
    /*const float2 dirt_hue = (float2) { dirt_hue_range.x, dirt_hue_range.y }; // 0, 360 };
    const float2 dirt_value = (float2) { dirt_hue_range.y, dirt_hue_range.z }; // 28, 36 };
    const float2 dirt_saturation = (float2) { dirt_hue_range.u, dirt_hue_range.v }; // 23, 33 };
    float3 dirt_hsv = generate_hsv_v_s(dirt_hue, dirt_value, dirt_saturation);*/

    /*const float2 grass_hue = (float2) { 0, 360 };
    const float2 grass_value = (float2) { 42, 66 };         // { 44, 54 };
    const float2 grass_saturation = (float2) { 44, 55 };    // { 38, 48 };
    float3 grass_hsv = generate_hsv_v_s(grass_hue, grass_value, grass_saturation);
    if (hsv_to_color(grass_hsv).r > hsv_to_color(dirt_hsv).r)
    {
        float soil_hue = dirt_hsv.x;
        dirt_hsv.x = grass_hsv.x;
        grass_hsv.x = soil_hue;
    }*/

    /*const int sand_hue_shift = 40;
    float3 sand_hsv = (float3) {
        dirt_hsv.x - sand_hue_shift + (rand() % sand_hue_shift * 2),
        18 + rand() % 8,
        54 + rand() % 16
    };*/
