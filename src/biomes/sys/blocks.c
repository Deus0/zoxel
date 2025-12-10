// TODO: On Generate Biome - Spawn Blocks

void BiomeBlocksSystem(iter *it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Generate);
    zox_sys_out(BlockLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Generate, generate);
        zox_sys_o(BlockLinks, blocks);

        if (generate->value != zox_dirty_active) {
            continue;
        }

        zox_sys_e();
        zox_log("Generating blocks for biome [%s]", zox_get_name(e));

        const entity dirt = spawn_block_soil(world, zox_block_dirt, "dirt", color_red);
        add_to_BlockLinks(blocks, dirt);
    }
} zoxd_system2(BiomeBlocksSystem);