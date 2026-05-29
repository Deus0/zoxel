// TODO: Refactor these into Biomes from Terrain
// Blocks >> Chunks >> Biomes >> Terrain ?

void generate_colors(lint seed, Colors *colors) {
    srand((unsigned int) seed);
    // One random seed color, then the rest are related off it.
    float3 dirt_hsv = (float3) {
        frand_range(0, 360),
        frand_range(14, 48),
        frand_range(10, 42)
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

// A biome will generate blocks
zox_sys2(BiomeBlocksSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Seed);
    zox_sys_out(Colors);
    zox_sys_out(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Seed, seed);
        zox_sys_o(Colors, colors);
        zox_sys_o(BlockLinks, blocks);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        zox_sys_e();
        resize_Colors(colors, realm_colors_count);
        generate_colors(seed->value, (colors));
        zox_logv("Generating blocks for biome [%s]", zox_get_name(e));
        byte j = 1;   // skip sky
        // const color sky_color = colors->value[j++];
        color dirt_color = colors->value[j++];
        color grass_color = colors->value[j++];
        //j++; // color sand_color = colors->value[j++];
        //j++; // color stone_color = colors->value[j++];
        //j++; // color obsidian_color = colors->value[j++];
        entity dirt = spawn_block_soil(world, 0, "dirt", dirt_color);
        zox_set(dirt, BiomeLink, { e });
        add_to_BlockLinks(blocks, dirt);
        entity soil_grass = spawn_block_soil_grass(world, 0, "soil_grass", dirt_color, grass_color);
        zox_set(soil_grass, BiomeLink, { e });
        add_to_BlockLinks(blocks, soil_grass);
        // zox_log("   - block at [%i]", realm_blocks->length);
        // add to biome as well
        // add blocks in biome to realm
        // set refresh then
    }
} zox_sys_end(BiomeBlocksSystem);
