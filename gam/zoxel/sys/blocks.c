// TODO: Remove [spawn_blocks_data] Hooks and just use systems
// NOTE: Spawns Realm Specific Blocks (Global ones)
zox_sys2(BlocksRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_out(BlockLinks);
    zox_sys_out(BlocksDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_o(BlockLinks, blocks);
        zox_sys_o(BlocksDirty, dirty);
        if (state->value != zox_generate_realm_blocks) {
            continue;
        }
        zox_log(" + [%f] Realm [blocks] [%i] spawning", zox_current_time, blocks->length);
        // Bottom of Realm
        {
            color obsidian_color = color_grayscale(rand_range(15, 35));
            entity e2 = spawn_block_stone(world, 0, "obsidian", obsidian_color);
            zox_add_tag(e2, BlockObsidian);
            zox_add_tag(e2, BlockInvinsible);
            zox_set_parent(world, e2, e);
            add_to_BlockLinks(blocks, e2);
        }
        // Crafted Items
        {
            // Bricks for Homes
            color bricks_color = color_grayscale(rand_range(50, 90));
            entity e2 = spawn_block_bricks(world, 0, "bricks", bricks_color);
            zox_add_tag(e2, BlockBricks);
            zox_set_parent(world, e2, e);
            add_to_BlockLinks(blocks, e2);
            zox_set(e2, BlockHealth, { (float2) { 10, 16 } });
        }
        // Decor Blocks
        {
            // Flowers for Gardens
            entity e2 = spawn_block_flower(world, 0);
            add_to_BlockLinks(blocks, e2);
        }
        spawn_dungeon_blocks(world, e, blocks);
        dirty->value = zox_dirty_trigger;
        zox_log(" + [%f] Realm [blocks] [%i] spawned", zox_current_time, blocks->length);
    }
} zox_sys_end(BlocksRealmSpawnSystem);
