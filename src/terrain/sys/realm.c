define_fun_stopwatch(time_realm_blocks, 0);

void clear_realm_blocks(ecs *world, const entity realm) {
    if (!realm) {
        return;
    }
    if (!zox_has(realm, BlockLinks)) {
        zox_log_error("realm does not have BlockLinks [%lu]", realm)
        return;
    }
    zox_muter(realm, BlockLinks, blocks);
    if (blocks) {
        for (int i = 0; i < blocks->length; i++) {
            zox_delete(blocks->value[i])
        }
    }
    // clear_BlockLinks(blocks);
    resize_BlockLinks(blocks, 0);
}

void spawn_realm_blocks(ecs *world, const entity realm) {
    if (!realm) {
        return;
    }
    if (!zox_has(realm, BlockLinks)) {
        zox_log_error("realm does not have BlockLinks [%lu]", realm)
        return;
    }
    startwatch(time_realm_blocks);

    zox_geter(realm, Colors, realm_colors);
    zox_geter(realm, ModelLinks, models);

    const byte target_depth = block_vox_depth;
    const byte texture_dim = powers_of_two_byte[target_depth];
    voxel_texture_size = (int2) { texture_dim, texture_dim };

    color dirt_color;
    color grass_color;
    color sand_color;
    color stone_color;
    color obsidian_color;
    if (realm_colors->length < 5 || grayscale_mode) {
        zox_log("+ grayscale_mode enabled")
        dirt_color = color_grayscale(88);
        grass_color = color_grayscale(144);
        sand_color = color_grayscale(166);
        stone_color = color_grayscale(45);
        obsidian_color = color_grayscale(13);
    } else {
        byte color_index = 1;   // skip sky
        // const color sky_color = realm_colors->value[color_index++];
        dirt_color = realm_colors->value[color_index++];
        grass_color = realm_colors->value[color_index++];
        sand_color = realm_colors->value[color_index++];
        stone_color = realm_colors->value[color_index++];
        obsidian_color = realm_colors->value[color_index++];
    }

    zox_muter(realm, BlockLinks, blocks);
    zox_log("Realm Blocks (Unique Start ID) [%i]", blocks->length);

    zox_block_sand = blocks->length + 1;
    entity sand = spawn_block_soil(world, zox_block_sand, "sand", sand_color);
    add_to_BlockLinks(blocks, sand);
    tapwatch(time_realm_blocks, "built sand");
    zox_log("Sand ID [%i]", zox_block_sand);

    zox_block_stone = blocks->length + 1;
    entity stone = spawn_block_stone(world, zox_block_stone, "stone", stone_color);
    add_to_BlockLinks(blocks, stone);
    tapwatch(time_realm_blocks, "built stone");
    zox_log("Stone ID [%i]", zox_block_stone);

    zox_block_obsidian = blocks->length + 1;
    entity obsidian = spawn_block_stone(world, zox_block_obsidian, "obsidian", obsidian_color);
    add_to_BlockLinks(blocks, obsidian);
    zox_add_tag(obsidian, BlockInvinsible);
    tapwatch(time_realm_blocks, "built obsidian");

    zox_block_bricks = blocks->length + 1;
    entity bricks = spawn_block_bricks(
        world,
        zox_block_bricks,
        "bricks",
        obsidian_color);
    add_to_BlockLinks(blocks, bricks);
    tapwatch(time_realm_blocks, "built bricks");

    // decor
    zox_block_vox_grass = blocks->length + 1;
    entity model_group_grass = models->length >= 1 ? models->value[0] : 0;
    entity grass = spawn_block_grass(
        world,
        zox_block_vox_grass,
        grass_color,
        model_group_grass
    );
    add_to_BlockLinks(blocks, grass);

    zox_block_dirt_rubble = blocks->length + 1;
    entity rubble = spawn_realm_block_rubble(
        world,
        zox_block_dirt_rubble,
        "rubble",
        dirt_color,
        vox_type_rubble
    );
    add_to_BlockLinks(blocks, rubble);
    tapwatch(time_realm_blocks, "built rubble");

    zox_block_dirt_flowers = blocks->length + 1;
    entity flowers = spawn_realm_block_rubble(
        world,
        zox_block_dirt_flowers,
        "flowers",
        dirt_color,
        vox_type_flowers
    );
    add_to_BlockLinks(blocks, flowers);
    tapwatch(time_realm_blocks, "built flowers");

    zox_block_dirt_vox = blocks->length + 1;
    entity pile = spawn_realm_block_noisey(world, zox_block_dirt_vox, "dirt pile", dirt_color);
    add_to_BlockLinks(blocks, pile);
    tapwatch(time_realm_blocks, "built pile");

    zox_block_vox_flower = blocks->length + 1;
    entity vox_flower = spawn_block_flower(world, zox_block_vox_flower);
    add_to_BlockLinks(blocks, vox_flower);
    tapwatch(time_realm_blocks, "built flower");

    spawn_blocks_data spawned_data = (spawn_blocks_data) {
        .realm = realm,
        .blocks = blocks,
    };
    run_hook_spawn_blocks(world, &spawned_data);
    tapwatch(time_realm_blocks, "ran hooks");


    for (int i = 0; i < blocks->length; i++) {
        if (!blocks->value[i]) {
            zox_log_error("[realm_blocks]: voxel invalid at [%i]", i)
        }
    }
    // zox_set_ptr(realm, BlockLinks, blocks);
    zox_set(realm, BlocksDirty, { zox_dirty_trigger });

    zox_logv("At [%f] Realm [blocks] [%i] spawned.", zox_current_time, blocks->length);
    endwatch(time_realm_blocks, "ending");
}