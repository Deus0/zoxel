// TODO: Refactor these into Biomes from Terrain
// Blocks >> Chunks >> Biomes >> Terrain ?
extern entity spawn_block_soil(ecs*, byte, char*, color);

extern entity spawn_block_soil_grass(ecs*, byte, char*, color, color);

// A biome will generate blocks
zox_sys2(BiomeBlocksSystem) {
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
        zox_logv("Generating blocks for biome [%s]", zox_get_name(e));

        byte j = 1;   // skip sky
        // const color sky_color = colors->value[j++];
        color dirt_color = colors->value[j++];
        color grass_color = colors->value[j++];
        //j++; // color sand_color = colors->value[j++];
        //j++; // color stone_color = colors->value[j++];
        //j++; // color obsidian_color = colors->value[j++];

        entity dirt = spawn_block_soil(world, 0, "dirt", dirt_color);
        zox_set(dirt, BiomeLink, { e });
        add_to_BlockLinks(blocks, dirt);

        entity soil_grass = spawn_block_soil_grass(world, 0, "soil_grass", dirt_color, grass_color);
        zox_set(soil_grass, BiomeLink, { e });
        add_to_BlockLinks(blocks, soil_grass);

        // zox_log("   - block at [%i]", realm_blocks->length);

        // add to biome as well

        // add blocks in biome to realm
        // set refresh then
    }
} zox_sys_end(BiomeBlocksSystem);
