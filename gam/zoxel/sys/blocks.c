// TOOD: Spawn a tilemap when blocks dirty / set old one
void spawn_realm_tilemaps(ecs *world, entity realm) {

    // spawn a tilemap!
    if (!zox_has(realm, TilemapLink)) {
        return;
    }
    zox_geter_value(realm, TilemapLink, entity, old);
    // if (old) return; // TODO: Temp; Remove when crashes gone

    if (zox_valid(old)) {
        zox_delete(old);
    }

    entity tilemap = spawn_tilemap(world, prefab_tilemap);
    zox_set(realm, TilemapLink, { tilemap });
    if (!tilemap) {
        return;
    }

    zox_set(tilemap, RealmLink, { realm });
    zox_set(realm, BlocksDirty, { zox_dirty_trigger });
}

// TODO: Remove [spawn_blocks_data] Hooks and just use systems
zox_sys2(BlocksRealmSpawnSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(GenerateRealm);
    zox_sys_in(Colors);
    zox_sys_in(ModelLinks);
    zox_sys_out(BlockLinks);
    zox_sys_out(BlocksDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(GenerateRealm, state);
        zox_sys_i(Colors, colors);
        zox_sys_i(ModelLinks, models);
        zox_sys_o(BlockLinks, blocks);
        zox_sys_o(BlocksDirty, dirty);
        if (state->value != zox_generate_realm_blocks) {
            continue;
        }
        color dirt_color;
        color grass_color;
        color sand_color;
        color stone_color;
        color obsidian_color;
        // color wood_color;
        if (colors->length < 5 || grayscale_mode) {
            zox_log("+ grayscale_mode enabled")
            dirt_color = color_grayscale(88);
            grass_color = color_grayscale(144);
            sand_color = color_grayscale(166);
            stone_color = color_grayscale(45);
            obsidian_color = color_grayscale(13);
            // wood_color = color_grayscale(122);
        } else {
            byte color_index = 1;   // skip sky
            // const color sky_color = colors->value[color_index++];
            dirt_color = colors->value[color_index++];
            grass_color = colors->value[color_index++];
            sand_color = colors->value[color_index++];
            stone_color = colors->value[color_index++];
            obsidian_color = colors->value[color_index++];
            // wood_color = colors->value[color_index++];
        }

        zox_block_sand = blocks->length + 1;
        entity sand = spawn_block_soil(world, zox_block_sand, "sand", sand_color);
        add_to_BlockLinks(blocks, sand);

        zox_block_stone = blocks->length + 1;
        entity stone = spawn_block_stone(world, zox_block_stone, "stone", stone_color);
        add_to_BlockLinks(blocks, stone);

        zox_block_obsidian = blocks->length + 1;
        entity obsidian = spawn_block_stone(world, zox_block_obsidian, "obsidian", obsidian_color);
        add_to_BlockLinks(blocks, obsidian);
        zox_add_tag(obsidian, BlockInvinsible);

        zox_block_bricks = blocks->length + 1;
        entity bricks = spawn_block_bricks(world, zox_block_bricks, "bricks", obsidian_color);
        add_to_BlockLinks(blocks, bricks);

        zox_block_wood = blocks->length + 1;
        entity wood = spawn_block_wood(world, zox_block_wood, "wood", obsidian_color);
        add_to_BlockLinks(blocks, wood);

        // decor
        zox_block_vox_grass = blocks->length + 1;
        // TODO: Spawn Model here instead
        entity model_group_grass = models->length >= 1 ? models->value[0] : 0;
        entity grass = spawn_block_grass(world, zox_block_vox_grass, grass_color, model_group_grass);
        add_to_BlockLinks(blocks, grass);

        zox_block_dirt_rubble = blocks->length + 1;
        entity rubble = spawn_realm_block_rubble(world, zox_block_dirt_rubble, "rubble", dirt_color, vox_type_rubble);
        add_to_BlockLinks(blocks, rubble);

        zox_block_dirt_flowers = blocks->length + 1;
        entity flowers = spawn_realm_block_rubble(world, zox_block_dirt_flowers, "flowers", dirt_color, vox_type_flowers);
        add_to_BlockLinks(blocks, flowers);

        zox_block_dirt_vox = blocks->length + 1;
        entity pile = spawn_realm_block_noisey(world, zox_block_dirt_vox, "dirt pile", dirt_color);
        add_to_BlockLinks(blocks, pile);

        zox_block_vox_flower = blocks->length + 1;
        entity vox_flower = spawn_block_flower(world, zox_block_vox_flower);
        add_to_BlockLinks(blocks, vox_flower);
        spawn_blocks_data spawned_data = (spawn_blocks_data) {
            .realm = e,
            .blocks = blocks,
        };
        run_hook_spawn_blocks(world, &spawned_data);
        dirty->value = zox_dirty_trigger;
        zox_logv("At [%f] Realm [blocks] [%i] spawned.", zox_current_time, blocks->length);
    }
} zox_sys_end(BlocksRealmSpawnSystem);
