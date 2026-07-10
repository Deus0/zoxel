// NOTE: Spawns Realm Specific Blocks (Global ones)
zox_sys2(BlocksRealmSpawnSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(BlocksDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(BlocksDirty, dirty);
        if (state->value != zox_generate_realm_blocks) {
            continue;
        }
        // Bottom of Realm
        {
            color obsidian_color = color_grayscale(rand_range(15, 35));
            // entity e2 = spawn_block_stone(world, e, 226661, "obsidian", obsidian_color);
            lint seed = 1166611;
            entity model = spawn_model_stone(world, e, seed, obsidian_color);
            entity e2 = spawn_realm_block_solid(world, prefab_block_vox_meta, e, seed,  "obsidian", obsidian_color, model);
            zox_add_tag(e2, BlockObsidian);
            zox_add_tag(e2, BlockInvinsible);
        }
        // Crafted Items
        {
            // Bricks for Homes
            color bricks_color = color_grayscale(rand_range(50, 90));
            lint seed = 232323;
            entity model = spawn_model_bricks(world, e, seed, bricks_color);
            entity e2 = spawn_realm_block_solid(world, prefab_block_vox_meta, e, seed,  "bricks", bricks_color, model);
            // entity e2 = spawn_block_bricks(world, e, "bricks", bricks_color, 232323);
            zox_add_tag(e2, BlockBricks);
            zox_set(e2, BlockHealth, { (float2) { 10, 16 } });
        }
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
        dirty->value = zox_dirty_trigger;
        if (dbg_log) {
            zox_log("Realm [%s] Spawning [Blocks]", zox_get_name(e));
        }
    }
} zox_sys_end(BlocksRealmSpawnSystem);
