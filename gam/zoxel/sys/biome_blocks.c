// TODO: Refactor these into Biomes from Terrain
// Blocks >> Chunks >> Biomes >> Terrain ?

void generate_colors(
    lint seed,
    Colors* colors,
    byte dbg_log)
{
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
    zox_sys_out(BiomeSkyColor);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Generate, generate);
        zox_sys_i(Seed, seed);
        zox_sys_o(BiomeSkyColor, sky);
        if (generate->value != zox_dirty_active) {
            continue;
        }
        // resize_Colors(colors, realm_colors_count);
        // generate_colors(seed->value, (colors), dbg_log);
        if (dbg_log) {
            zox_log("Generating blocks for biome [%s] - Seed [%lu]", zox_get_name(e), seed->value);
        }
        // TODO: Set Biome Sky Color here
        // byte j = 2;   // skip sky
        uint seed_shift = 3690;
        uint variant_shift = 369;
        uint seed_offset = seed_shift;
        lint sky_seed = seed->value + (seed_offset+=seed_shift);
        sky->value = seed_to_color_rgb_range(
            sky_seed,
            (float2) { 0, 360 },
            (float2) { 30, 70 },
            (float2) { 30, 70 });
        lint dirt_seed = seed->value + (seed_offset+=seed_shift);
        color dirt_color = color_rgb_to_color(
            seed_to_color_rgb_range(
                dirt_seed,
                (float2) { 0, 360 },
                (float2) { 10, 90 },
                (float2) { 25, 75 }));
        lint grass_seed = seed->value + (seed_offset+=seed_shift);
        color grass_color = color_rgb_to_color(
            seed_to_color_rgb_range(
                grass_seed,
                (float2) { 0, 360 },
                (float2) { 10, 90 },
                (float2) { 30, 70 }));
        lint sand_seed = seed->value + (seed_offset+=seed_shift);
        color sand_color = color_rgb_to_color(
            seed_to_color_rgb_range(
                sand_seed,
                (float2) { 0, 360 },
                (float2) { 35, 70 },
                (float2) { 40, 75 }));
        lint stone_seed = seed->value + (seed_offset+=seed_shift);
        color stone_color = color_rgb_to_color(
            seed_to_color_rgb_range(stone_seed,
                (float2) { 0, 360 },
                (float2) { 15, 45 },
                (float2) { 30, 60 }));
        lint wood_seed = seed->value + (seed_offset+=seed_shift);
        color wood_color = color_rgb_to_color(
            seed_to_color_rgb_range(
                wood_seed,
                (float2) { 0, 360 },
                (float2) { 10, 90 },
                (float2) { 20, 50 }));
        lint road_seed = seed->value + (seed_offset+=seed_shift);
        color road_color = color_rgb_to_color(
            seed_to_color_rgb_range(
                road_seed,
                (float2) { 0, 360 },
                (float2) { 10, 40 },
                (float2) { 20, 50 }));
        lint flowers_seed = seed->value + (seed_offset+=seed_shift);
        color flowers_color = color_rgb_to_color(
            seed_to_color_rgb_range(
                flowers_seed,
                (float2) { 0, 360 },
                (float2) { 10, 90 },
                (float2) { 30, 70 }));
        {
            entity model = spawn_model_soil(
                world,
                "dirt",
                e,
                dirt_seed,
                dirt_color,
                0.44f,
                vox_type_soil);
            entity block = spawn_realm_block_solid(
                world,
                prefab_block_baked,
                e,
                dirt_seed,
                "dirt",
                dirt_color,
                model,
                dbg_log);
            zox_add(block, BlockSoil);
            zox_setv(block, BlockHealth,
                ((float2) { 3, 6 }));
        }
        {
            entity model = spawn_model_soil(
                world,
                "sand",
                e,
                sand_seed,
                sand_color,
                0.14f,
                vox_type_sand);
            entity block = spawn_realm_block_solid(
                world,
                prefab_block_baked,
                e,
                sand_seed,
                "sand",
                sand_color,
                model,
                dbg_log);
            zox_add(block, BlockSand);
            zox_setv(block, BlockHealth,
                ((float2) { 2, 4 }));
        }
        {
            entity model = spawn_model_stone(
                world,
                e,
                stone_seed,
                stone_color);
            entity block = spawn_realm_block_solid(
                world,
                prefab_block_baked,
                e,
                stone_seed,
                "stone",
                stone_color,
                model,
                dbg_log);
            zox_add(block, BlockStone);
            zox_setv(block, BlockHealth,
                     ((float2) { 8, 12 }));
        }
        {
            entity model = spawn_model_wood(
                world,
                e,
                wood_seed,
                wood_color);
            entity block = spawn_realm_block_solid(
                world,
                prefab_block_baked,
                e,
                wood_seed,
                "wood",
                wood_color,
                model,
                dbg_log);
            zox_add(block, BlockWood);
            zox_setv(block, BlockHealth,
                ((float2) { 4, 8 }));
        }
        {
            // color road_color = color_mix(dirt_color, stone_color, 0.4f);
            entity model = spawn_model_road(
                world,
                e,
                road_seed,
                road_color);
            entity block = spawn_realm_block_solid(
                world,
                prefab_block_baked,
                e,
                road_seed,
                "road",
                road_color,
                model,
                dbg_log);
            zox_add(block, BlockRoad);
            zox_set(block, BlockHealth, { (float2) { 6, 10 } });
        }
        {
            entity model = spawn_model_soil_grass(
                world,
                e,
                grass_seed,
                dirt_color,
                grass_color,
                0.44f);
            entity block = spawn_realm_block_solid(
                world,
                prefab_block_baked,
                e,
                grass_seed,
                "soil_grass",
                grass_color,
                model,
                dbg_log);
            zox_add(block, BlockSoilGrass);
            zox_set(block, BlockHealth, { (float2) { 4, 8 } });
        }
        // Grass Model
        {
            byte max_depth = block_depth_limits.y;
            color weed_color = color_mix(grass_color, stone_color, 0.8f);
            byte grass_color_mutation = 15;
            // create model group for grass
            entity model_group = zox_ins(world, prefab_model_group);
            zox_set_unique_name(model_group, "model_group_grass");
            zox_setv(model_group, MaxRenderDepth, max_depth);
            zox_set_parent(world, model_group, e);
            // entity2 variant = (entity2) { 0 };
            entity texture_vox = 0;
            ModelLinks variants = (ModelLinks) { 0 };
            for (int j = 0; j < grass_variants; j++) {
                lint variant_seed = grass_seed +
                    variant_shift * (j + 1);
                color variant_color = weed_color;
                srand(variant_seed);
                variant_color = color_mutate(variant_color, grass_color_mutation);
                entity2 e2 = spawn_model_grass(
                    world,
                    prefab_vox,
                    variant_seed,
                    max_depth,
                    variant_color);
                entity model = e2.x;
                zox_set_parent(world, model, model_group);
                add_to_ModelLinks(&variants, model);
                // variant = e2;
                if (j == 0) {
                    texture_vox = e2.y;
                }
            }
            zox_set_ptr(model_group, ModelLinks, variants);
            // entity texture_vox = variant.y;
            entity block = spawn_realm_block_model(
                world,
                e,
                grass_seed,
                "grass",
                weed_color,
                0,
                model_group,
                max_depth,
                texture_vox,
                direction_front);
            zox_add(block, BlockGrass);
            zox_setv(block, BlockLightPass, 1);
            zox_setv(block, BlockSound, 1);
        }
        // Dirt Piles on ground
        {
            byte max_depth = block_depth_limits.y;
            entity2 e2 = spawn_model_lods_generated(
                world,
                prefab_vox,
                e,
                "debris",
                vox_type_rubble,
                dirt_color,
                max_depth,
                dirt_seed);
            entity model = e2.x;
            entity texture_vox = e2.y;
            entity block = spawn_realm_block_model(
                world,
                e,
                dirt_seed,
                "debris",
                dirt_color,
                0,
                model,
                max_depth,
                texture_vox,
                direction_up);
            zox_set(block, BlockLightPass, { 1 });
        }
        // A noisey block
        {
            byte max_depth = block_depth_limits.y;
            entity2 e2 = spawn_model_lods_generated(
                world,
                prefab_vox,
                e,
                "decayed",
                vox_type_noisey,
                dirt_color,
                max_depth,
                dirt_seed);
            entity model = e2.x;
            entity texture_model = e2.y;
            spawn_realm_block_model(
                world,
                e,
                dirt_seed,
                "decayed",
                dirt_color,
                1,
                model,
                max_depth,
                texture_model,
                direction_front);
        }
        // Biome Flora
        {
            char* block_name = "stalks";
            byte max_depth = block_depth_limits.y;
            entity2 e2 = spawn_model_lods_generated(
                world,
                prefab_vox,
                e,
                block_name,
                vox_type_flowers,
                flowers_color,
                max_depth,
                flowers_seed);
            entity model = e2.x;
            entity texture_model = e2.y;
            entity block = spawn_realm_block_model(
                world,
                e,
                flowers_seed,
                block_name,
                flowers_color,
                0,
                model,
                max_depth,
                texture_model,
                direction_front);
            zox_add(block, BlockFlower);
            zox_set(block, BlockLightPass, { 1 });
            zox_set(block, BlockSound, { 1 });
        }
        if (dbg_log) {
            zox_log("+ Biome [%s] Blocks", zox_get_name(e));
        }
    }
} zox_sys_end(BiomeBlocksSystem);
