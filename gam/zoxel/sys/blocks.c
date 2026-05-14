// TODO: Remove [spawn_blocks_data] Hooks and just use systems
// Unique Realm Blocks spawn here
zox_sys2(BlocksRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(ModelLinks);
    zox_sys_out(BlockLinks);
    zox_sys_out(BlocksDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(ModelLinks, models);
        zox_sys_o(BlockLinks, blocks);
        zox_sys_o(BlocksDirty, dirty);
        if (state->value != zox_generate_realm_blocks) {
            continue;
        }
        color dirt_color = color_grayscale(88);
        color grass_color = color_grayscale(144);
        color sand_color = color_grayscale(166);
        color stone_color = color_grayscale(45);
        color obsidian_color = color_grayscale(13);
        color wood_color = color_grayscale(111);
        color bricks_color = color_grayscale(60);

        {
            zox_block_sand = blocks->length + 1;
            entity e2 = spawn_block_soil(world, zox_block_sand, "sand", sand_color);
            add_to_BlockLinks(blocks, e2);
        }
        {
            zox_block_stone = blocks->length + 1;
            entity e2 = spawn_block_stone(world, zox_block_stone, "stone", stone_color);
            add_to_BlockLinks(blocks, e2);
        }
        {
            zox_block_obsidian = blocks->length + 1;
            entity e2 = spawn_block_stone(world, zox_block_obsidian, "obsidian", obsidian_color);
            zox_add_tag(e2, BlockInvinsible);
            add_to_BlockLinks(blocks, e2);
        }
        {
            zox_block_bricks = blocks->length + 1;
            entity e2 = spawn_block_bricks(world, zox_block_bricks, "bricks", bricks_color);
            add_to_BlockLinks(blocks, e2);
        }
        {
            zox_block_wood = blocks->length + 1;
            entity e2 = spawn_block_wood(world, zox_block_wood, "wood", wood_color);
            add_to_BlockLinks(blocks, e2);
        }
        // decor
        zox_block_vox_grass = blocks->length + 1;
        // TODO: Spawn Model here instead
        {
            entity model_group_grass = models->length >= 1 ? models->value[0] : 0;
            entity e2 = spawn_block_grass(world, zox_block_vox_grass, grass_color, model_group_grass);
            add_to_BlockLinks(blocks, e2);
        }
        {
            zox_block_dirt_rubble = blocks->length + 1;
            entity e2 = spawn_realm_block_rubble(world, zox_block_dirt_rubble, "rubble", dirt_color, vox_type_rubble);
            add_to_BlockLinks(blocks, e2);
        }
        {
            zox_block_dirt_flowers = blocks->length + 1;
            entity e2 = spawn_realm_block_rubble(world, zox_block_dirt_flowers, "flowers", dirt_color, vox_type_flowers);
            add_to_BlockLinks(blocks, e2);
        }
        {
            zox_block_dirt_vox = blocks->length + 1;
            entity e2 = spawn_realm_block_noisey(world, zox_block_dirt_vox, "dirt pile", dirt_color);
            add_to_BlockLinks(blocks, e2);
        }
        {
            zox_block_vox_flower = blocks->length + 1;
            entity e2 = spawn_block_flower(world, zox_block_vox_flower);
            add_to_BlockLinks(blocks, e2);
        }
        spawn_blocks_data spawned_data = (spawn_blocks_data) {
            .realm = e,
            .blocks = blocks,
        };
        run_hook_spawn_blocks(world, &spawned_data);
        dirty->value = zox_dirty_trigger;
        zox_logv("At [%f] Realm [blocks] [%i] spawned.", zox_current_time, blocks->length);
    }
} zox_sys_end(BlocksRealmSpawnSystem);
