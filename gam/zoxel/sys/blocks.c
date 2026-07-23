// NOTE: Spawns Realm Specific Blocks (Global ones)
zox_sys2(BlocksRealmSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Seed);
    zox_sys_out(BlocksDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Seed, seed);
        zox_sys_o(BlocksDirty, dirty);
        if (state->value != zox_generate_realm_blocks) {
            continue;
        }
        uint seed_shift = 3690;
        uint seed_offset = seed_shift * 100; // make sure different to biome seeds
        // Bottom of Realm
        {
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            color block_color = color_rgb_to_color(seed_to_color_rgb_range(block_seed, (float2) { 0, 360 }, (float2) { 25, 50 }, (float2) { 10, 40 }));
            // color obsidian_color = color_grayscale(rand_range(15, 35));
            // entity e2 = spawn_block_stone(world, e, 226661, "obsidian", obsidian_color);
            entity model = spawn_model_stone(world, e, block_seed, block_color);
            entity e2 = spawn_realm_block_solid(world, prefab_block_vox_meta, e, block_seed,  "obsidian", block_color, model, dbg_log);
            zox_add_tag(e2, BlockObsidian);
            zox_add_tag(e2, BlockInvinsible);
        }
        // Crafted Items
        {
            // Bricks for Homes
            lint block_seed = seed->value + (seed_offset+=seed_shift);
            color bricks_color = color_rgb_to_color(seed_to_color_rgb_range(block_seed, (float2) { 0, 360 }, (float2) { 30, 70 }, (float2) { 25, 45 }));
            color cracks_color = color_rgb_to_color(seed_to_color_rgb_range(block_seed + 100, (float2) { 0, 360 }, (float2) { 0, 100 }, (float2) { 5, 15 }));
            // color bricks_color = color_grayscale(rand_range(50, 90));
            entity model = spawn_model_bricks(world, e, block_seed, bricks_color, cracks_color);
            entity e2 = spawn_realm_block_solid(world, prefab_block_vox_meta, e, block_seed, "bricks", bricks_color, model, dbg_log);
            // entity e2 = spawn_block_bricks(world, e, "bricks", bricks_color, 232323);
            zox_add_tag(e2, BlockBricks);
            zox_set(e2, BlockHealth, { (float2) { 10, 16 } });
        }
        // === File Blocks ===
        // Decor Blocks
        {
            // Flowers for Gardens
            entity model = string_hashmap_get(files_hashmap_voxes, new_string_data("flower"));
            if (model) {
                zox_geter(model, ModelLods, lods);
                byte max_render_depth = block_vox_depth;
                entity vox = lods->value[max_render_depth];
                entity e2 = spawn_realm_block_model(world, e, 123361, "rose", color_red, 0, model, vox, direction_up);
                zox_set(e2, BlockLightPass, { 1 });
            }
        }
        spawn_dungeon_blocks(world, e);
        dirty->value = zox_blocks_dirty_start;
        if (dbg_log) {
            zox_log("Realm [%s] Spawning [Blocks]", zox_get_name(e));
        }
    }
} zox_sys_end(BlocksRealmSpawnSystem);
