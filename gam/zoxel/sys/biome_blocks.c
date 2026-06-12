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

// TODO: Generate Block Seeds off Biome Seeds
// A biome will generate blocks
zox_sys2(BiomeBlocksSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Seed);
    zox_sys_out(Colors);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(Seed, seed);
        zox_sys_o(Colors, colors);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        resize_Colors(colors, realm_colors_count);
        generate_colors(seed->value, (colors));
        zox_logv("Generating blocks for biome [%s]", zox_get_name(e));
        byte j = 1;   // skip sky
        // const color sky_color = colors->value[j++];
        color dirt_color = colors->value[j++];
        color grass_color = colors->value[j++];
        color sand_color = colors->value[j++];
        //j++; // color stone_color = colors->value[j++];
        //j++; // color obsidian_color = colors->value[j++];
        {
            entity e2 = spawn_block_soil(world, e, "dirt", dirt_color, 222122);
            zox_set(e2, BiomeLink, { e });
            zox_add_tag(e2, BlockSoil);
            zox_set(e2, BlockHealth, { (float2) { 3, 6 } });
        }
        {
            entity e2 = spawn_block_soil_grass(world, e, "soil_grass", dirt_color, grass_color, 662662);
            zox_set(e2, BiomeLink, { e });
            zox_add_tag(e2, BlockSoilGrass);
            zox_set(e2, BlockHealth, { (float2) { 4, 8 } });
        }
        {
            // color sand_color = color_grayscale(166);
            entity e2 = spawn_block_soil(world, e, "sand", sand_color, 515111);
            zox_add_tag(e2, BlockSand);
            zox_set(e2, BlockHealth, { (float2) { 2, 4 } });
        }
        {
            color stone_color = color_grayscale(45);
            entity e2 = spawn_block_stone(world, 0, "stone", stone_color);
            zox_add_tag(e2, BlockStone);
            zox_set_parent(world, e2, e);
            zox_set(e2, BlockHealth, { (float2) { 8, 12 } });
        }
        {
            // zox_block_wood = blocks->length + 1;
            color wood_color = color_grayscale(111);
            entity e2 = spawn_block_wood(world, 0, "wood", wood_color);
            zox_add_tag(e2, BlockWood);
            zox_set_parent(world, e2, e);
            zox_set(e2, BlockHealth, { (float2) { 4, 8 } });
        }
        // Grass Model
        {
            byte grass_color_mutation = 20;
            // Spawn Model First
            byte mdepth_vode = block_vox_depth_limits.y;
            entity model = zox_new();
            zox_set_unique_name(model, "model_group_grass");
            // parent the model to the biome
            zox_set_parent(world, model, e);
            ModelLinks variants = (ModelLinks) { 0 };
            entity2 variant;
            for (int j = 0; j < grass_variants; j++) {
                lint seed = 369 * j;
                color variant_color = grass_color;
                srand(seed);
                variant_color = color_mutate(variant_color, grass_color_mutation);
                entity2 e3 = spawn_model_grass(world, seed, mdepth_vode, variant_color);
                add_to_ModelLinks(&variants, e3.x);
                variant = e3;
            }
            zox_set_ptr(model, ModelLinks, variants);
            // Now spawn block
            // entity e3 = spawn_block_grass(world, e, grass_color, model, variant.y, 12331);
            entity e3 = spawn_realm_block_model(world, e, 12331, "grass", grass_color, 0, model, variant.y);
            zox_prefab_set(e3, BlockSound, { 1 });
            zox_add_tag(e3, BlockGrass);
        }
        // Dirt Piles on ground
        {
            lint seed = 322232;
            byte max_depth = block_vox_depth_limits.y;
            entity2 e2 = spawn_model_lods_rubble(world, e, vox_type_rubble, dirt_color, max_depth, seed);
            spawn_realm_block_model(world, e, seed, "dirt debris", dirt_color, 0, e2.x, e2.y);
        }
        // A noisey block
        {
            lint seed = 291911;
            byte max_depth = block_vox_depth_limits.y;
            entity2 e2 = spawn_model_lods_rubble(world, e, vox_type_noisey, dirt_color, max_depth, seed);
            spawn_realm_block_model(world, e, seed, "dirt decayed", dirt_color, 1, e2.x, e2.y);
        }
        // Biome Flora
        {
            lint seed = 11121;
            byte max_depth = block_vox_depth_limits.y;
            entity2 e2 = spawn_model_lods_rubble(world, e, vox_type_flowers, dirt_color, max_depth, seed);
            spawn_realm_block_model(world, e, seed, "flowers", dirt_color, 0,  e2.x, e2.y);
        }
        zox_logv("+ Biome [%s] Blocks", zox_get_name(e));
        // add to biome as well
        // add blocks in biome to realm
        // set refresh then
    }
} zox_sys_end(BiomeBlocksSystem);
