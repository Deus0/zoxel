extern double terrain_frequency;

byte realm_colors_count = 8;

static inline float wrap_hue(float h) {
    while (h < 0.0f) h += 360.0f;
    while (h >= 360.0f) h -= 360.0f;
    return h;
}

static inline float frand_range(float a, float b) {
    return a + ((float)rand() / (float)RAND_MAX) * (b - a);
}

static inline float3 hsv_shift(float3 hsv, float dh, float ds, float dv) {
    hsv.x = wrap_hue(hsv.x + dh);
    hsv.y = clampf(hsv.y + ds, 0.0f, 100.0f);
    hsv.z = clampf(hsv.z + dv, 0.0f, 100.0f);
    return hsv;
}

void generate_colors(lint seed, Colors *colors) {
    srand((unsigned int) seed);
    // One random seed color, then the rest are related off it.
    float3 dirt_hsv = (float3) {
        frand_range(0, 360),
        frand_range(10, 90),
        frand_range(10, 90)
    };
    // Grass: same family, more alive.
    float3 grass_hsv = hsv_shift(
        dirt_hsv,
        frand_range(55.0f, 105.0f),
        frand_range(8.0f, 25.0f),
        frand_range(4.0f, 18.0f)
    );
    // Sand: dirt warmed and bleached a bit.
    float3 sand_hsv = hsv_shift(
        dirt_hsv,
        frand_range(-12.0f, 18.0f),
        frand_range(-18.0f, -4.0f),
        frand_range(18.0f, 35.0f)
    );
    // Stone: the same note, stripped down and quiet.
    float3 stone_hsv = hsv_shift(
        dirt_hsv,
        frand_range(-10.0f, 10.0f),
        frand_range(-55.0f, -25.0f),
        frand_range(8.0f, 22.0f)
    );
    // Obsidian: opposite hue, low saturation, low value.
    float3 obsidian_hsv = hsv_shift(
        dirt_hsv,
        180.0f + frand_range(-25.0f, 25.0f),
        frand_range(-65.0f, -40.0f),
        frand_range(-35.0f, -18.0f)
    );
    // Sky: far away, softer, brighter, slightly displaced.
    float3 sky_hsv = hsv_shift(
        dirt_hsv,
        180.0f + frand_range(-45.0f, 45.0f),
        frand_range(-45.0f, -15.0f),
        frand_range(20.0f, 45.0f)
    );
    // Wood: dirt with a sunburn and a little age.
    float3 wood_hsv = hsv_shift(
        dirt_hsv,
        frand_range(8.0f, 28.0f),
        frand_range(-18.0f, 4.0f),
        frand_range(4.0f, 18.0f)
    );
    // Limit our value within a visually safe range
    // NOTE: too dark and we cannot see
    float vmin = 15;
    float vmax = 90;
    dirt_hsv.z = clampf(dirt_hsv.z, vmin, vmax);
    grass_hsv.z = clampf(grass_hsv.z, vmin, vmax);
    sand_hsv.z = clampf(sand_hsv.z, vmin, vmax);
    stone_hsv.z = clampf(stone_hsv.z, vmin, vmax);
    sky_hsv.z = clampf(sky_hsv.z, vmin, vmax);
    obsidian_hsv.z = clampf(obsidian_hsv.z, vmin, vmax);
    wood_hsv.z = clampf(wood_hsv.z, vmin, vmax);
    // Convert colors from hues
    color dirt_color = hsv_to_color(dirt_hsv);
    color grass_color = hsv_to_color(grass_hsv);
    color sand_color = hsv_to_color(sand_hsv);
    color stone_color = hsv_to_color(stone_hsv);
    color sky_color = hsv_to_color(sky_hsv);
    color obsidian_color = hsv_to_color(obsidian_hsv);
    color wood_color = hsv_to_color(wood_hsv);
    // add to our array
    byte i = 0;
    colors->value[i++] = sky_color;
    colors->value[i++] = dirt_color;
    colors->value[i++] = grass_color;
    colors->value[i++] = sand_color;
    colors->value[i++] = stone_color;
    colors->value[i++] = obsidian_color;
    colors->value[i++] = wood_color;
    // pick frequency now
    terrain_frequency = randf_range(0.003f, 0.019f);
    // BiomeData biome = pick_biome(seed);
    //terrain_frequency = biome.frequency;
}

/*void generate_colors_old(lint seed, Colors *colors) {

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
}*/

// extern void set_camera_fog_color(ecs*, color_rgb) ;



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
