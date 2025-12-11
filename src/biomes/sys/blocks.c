void BiomeBlocksSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(Colors);
    zox_sys_out(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(Colors, colors);
        zox_sys_o(BlockLinks, blocks);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        zox_sys_e();
        zox_log("Generating blocks for biome [%s]", zox_get_name(e));

        byte j = 1;   // skip sky
        // const color sky_color = colors->value[j++];
        color dirt_color = colors->value[j++];
        color grass_color = colors->value[j++];
        color sand_color = colors->value[j++];
        color stone_color = colors->value[j++];
        color obsidian_color = colors->value[j++];

        // zox_geter(realm->value, BlockLinks, realm_blocks);

        const entity dirt = spawn_block_soil(
            world,
            0,
            "dirt",
            dirt_color
        );
        zox_set(dirt, BiomeLink, { e });
        add_to_BlockLinks(blocks, dirt);

        entity soil_grass = spawn_block_soil_grass(
            world,
            0,
            "soil_grass",
            dirt_color,
            grass_color
        );
        zox_set(soil_grass, BiomeLink, { e });
        add_to_BlockLinks(blocks, soil_grass);

        // zox_log("   - block at [%i]", realm_blocks->length);

        // add to biome as well

        // add blocks in biome to realm
        // set refresh then
    }
} zoxd_system2(BiomeBlocksSystem);

void BiomeBlocks2System(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_in(RealmLink);
    zox_sys_in(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_i(RealmLink, realm);
        zox_sys_i(BlockLinks, blocks);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        zox_muter(realm->value, BlockLinks, realm_blocks);
        for (int j = 0; j < blocks->length; j++) {
            const entity block = blocks->value[j];
            add_to_BlockLinks(realm_blocks, block);
            // set on block
            byte index = realm_blocks->length;
            zox_set(block, BlockIndex, { index });
            if (j == 0) {
                zox_block_dirt = index;
                zox_log("+ Set zox_block_dirt [%i]", index);
            } else if (j == 1) {
                zox_block_dirt_grass = index;
                zox_log("+ Set zox_block_dirt_grass [%i]", index);
            }
        }
    }
} zoxd_system2(BiomeBlocks2System);