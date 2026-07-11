// TODO: Refactor these into Biomes from Terrain
// Blocks >> Chunks >> Biomes >> Terrain ?

void generate_colors(lint seed, Colors* colors, byte dbg_log) {
    srand((uint) seed);
    // One random seed color, then the rest are related off it
    float3 dirt_hsv = (float3) {
        frand_range(0, 360),
        frand_range(18, 48),
        frand_range(10, 36)
    };
    // Grass: same family, more alive.
    float3 grass_hsv = hsv_shift(
        dirt_hsv,
        rand_range(0, 100) >= 50 ? frand_range(160, 200) : frand_range(-200, -160),
        // frand_range(115.0f, 235.0f),
        frand_range(8.0f, 25.0f),
        frand_range(18.0f, 42.0f)
    );
    // Sand: dirt warmed and bleached a bit
    float2 sand_shift = (float2) { 30, 60 };
    float3 sand_hsv = hsv_shift(
        dirt_hsv,
        rand_range(0, 100) >= 50 ? frand_range(sand_shift.x, sand_shift.y) : frand_range(-sand_shift.y, -sand_shift.x),
        frand_range(22.0f, 36.0f),
        frand_range(22.0f, 36.0f)
    );
    // Stone: the same note, stripped down and quiet.
    float3 stone_hsv = hsv_shift(
        dirt_hsv,
        frand_range(-35, 35),
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
    float vmin = 10;
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
    colors->value[i++] = obsidian_color;
    colors->value[i++] = dirt_color;
    colors->value[i++] = grass_color;
    colors->value[i++] = sand_color;
    colors->value[i++] = stone_color;
    colors->value[i++] = wood_color;
    // pick frequency now
    terrain_frequency = randf_range(0.003f, 0.019f);
    // BiomeData biome = pick_biome(seed);
    //terrain_frequency = biome.frequency;
    if (dbg_log) {
        zox_log(" - Dirt HSV [%.1fx%.1fx%.1f]", dirt_hsv.x, dirt_hsv.y, dirt_hsv.z);
        zox_log(" - Grass HSV [%.1fx%.1fx%.1f]", grass_hsv.x, grass_hsv.y, grass_hsv.z);
        zox_log(" - Sand HSV [%.1fx%.1fx%.1f]", sand_hsv.x, sand_hsv.y, sand_hsv.z);
        zox_log(" - Stone HSV [%.1fx%.1fx%.1f]", stone_hsv.x, stone_hsv.y, stone_hsv.z);
        zox_log(" - Wood HSV [%.1fx%.1fx%.1f]", wood_hsv.x, wood_hsv.y, wood_hsv.z);
    }
}

// TODO: Generate Block Seeds off Biome Seeds
// A biome will generate blocks
zox_sys2(BiomeBlocksSystem) {
    byte dbg_log = 0;
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
        generate_colors(seed->value, (colors), dbg_log);
        if (dbg_log) {
            zox_log("Generating blocks for biome [%s] - Seed [%lu]", zox_get_name(e), seed->value);
        }
        // TODO: Set Biome Sky Color here
        byte j = 2;   // skip sky
        color dirt_color = colors->value[j++];
        color grass_color = colors->value[j++];
        color sand_color = colors->value[j++];
        color stone_color = colors->value[j++];
        color wood_color = colors->value[j++];
        {
            lint seed = 222122;
            entity model = spawn_model_soil(world, e, seed, dirt_color, 0.44f);
            entity block = spawn_realm_block_solid(world, prefab_block_vox_meta, e, seed,  "dirt", dirt_color, model);
            zox_add_tag(block, BlockSoil);
            zox_set(block, BlockHealth, { (float2) { 3, 6 } });
            // zox_set(block, BiomeLink, { e });
        }
        {
            lint seed = 515111;
            entity model = spawn_model_soil(world, e, seed, sand_color, 0.14f);
            entity block = spawn_realm_block_solid(world, prefab_block_vox_meta, e, seed,  "sand", sand_color, model);
            zox_add_tag(block, BlockSand);
            zox_set(block, BlockHealth, { (float2) { 2, 4 } });
        }
        {
            // entity e2 = spawn_block_stone(world, e, 222111, "stone", stone_color);
            lint seed = 222155;
            entity model = spawn_model_stone(world, e, seed, stone_color);
            entity block = spawn_realm_block_solid(world, prefab_block_vox_meta, e, seed,  "stone", stone_color, model);
            zox_add_tag(block, BlockStone);
            zox_set(block, BlockHealth, { (float2) { 8, 12 } });
        }
        {
            lint seed = 113321;
            entity model = spawn_model_wood(world, e, seed, wood_color);
            entity block = spawn_realm_block_solid(world, prefab_block_vox_meta, e, seed,  "wood", wood_color, model);
            zox_add_tag(block, BlockWood);
            zox_set(block, BlockHealth, { (float2) { 4, 8 } });
        }
        {
            lint seed = 662662;
            entity model = spawn_model_soil_grass(world, e, seed, dirt_color, grass_color, 0.44f);
            entity block = spawn_realm_block_solid(world, prefab_block_vox_meta, e, seed,  "soil_grass", dirt_color, model);
            zox_add_tag(block, BlockSoilGrass);
            zox_set(block, BlockHealth, { (float2) { 4, 8 } });
            // zox_set(block, BiomeLink, { e });
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
            entity2 variant = (entity2) { 0, 0 };
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
            entity texture_vox = variant.y;
            entity block = spawn_realm_block_model(world, e, 12331, "grass", grass_color, 0, model, texture_vox, direction_front);
            zox_add_tag(block, BlockGrass);
            zox_set(block, BlockLightPass, { 1 });
            zox_set(block, BlockSound, { 1 });
        }
        // Dirt Piles on ground
        {
            lint seed = 322232;
            byte max_depth = block_vox_depth_limits.y;
            entity2 e2 = spawn_model_lods_generated(world, e, vox_type_rubble, dirt_color, max_depth, seed);
            entity texture_vox = e2.y;
            entity block = spawn_realm_block_model(world, e, seed, "debris", dirt_color, 0, e2.x, texture_vox, direction_up);
            zox_set(block, BlockLightPass, { 1 });
        }
        // A noisey block
        {
            lint seed = 291911;
            byte max_depth = block_vox_depth_limits.y;
            entity2 e2 = spawn_model_lods_generated(world, e, vox_type_noisey, dirt_color, max_depth, seed);
            entity texture_vox = e2.y;
            spawn_realm_block_model(world, e, seed, "decayed", dirt_color, 1, e2.x, texture_vox, direction_front);
        }
        // Biome Flora
        {
            lint seed = 11121;
            byte max_depth = block_vox_depth_limits.y;
            entity2 e2 = spawn_model_lods_generated(world, e, vox_type_flowers, dirt_color, max_depth, seed);
            entity texture_vox = e2.y;
            entity block = spawn_realm_block_model(world, e, seed, "flowers", dirt_color, 0, e2.x, texture_vox, direction_up);
            zox_add_tag(block, BlockFlower);
            zox_set(block, BlockLightPass, { 1 });
            zox_set(block, BlockSound, { 1 });
        }
        if (dbg_log) {
            zox_log("+ Biome [%s] Blocks", zox_get_name(e));
        }
    }
} zox_sys_end(BiomeBlocksSystem);
