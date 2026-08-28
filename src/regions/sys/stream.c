// NOTE: SPawn in regions
/*zox_sys2(RegionSpawnSystem) {
    byte dbg_log = 0;
    byte distance = terrain_lod_far;
    zox_sys_query();
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(NodeDepth);
    zox_sys_out(RegionLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(NodeDepth, depth);
        zox_sys_o(RegionLinks, regions);
        short chunk_length = octree_size(depth->value);
        zox_sys_query_begin();
        while (zox_sys_query_loop()) {
            zox_sys_begin_2();
            zox_sys_in_2(StreamDirty2);
            zox_sys_in_2(StreamPosition2);
            for (int j = 0; j < it2.count; j++) {
                // zox_sys_e_2();
                zox_sys_i_2(StreamDirty2, dirty)
                zox_sys_i_2(StreamPosition2, position);
                if (dirty->value != zox_dirty_active) {
                    continue;
                }
                // Spawn region for all positions within
                for (int x = -distance; x <= distance; x++) {
                    for (int y = -distance; y <= distance; y++) {
                        int2 tunk_position = (int2) {
                            (position->value.x + x),
                            (position->value.y + y)
                        };
                        int2 region_position = tunk_position_to_region_position(tunk_position);
                        // zox_log("New Region [%ix%i] Stream Position [%ix%i]", region_position.x, region_position.y, position->value.x, position->value.y);
                        if (!int2_hashmap_has(regions->value, region_position)) {
                            if (dbg_log) {
                                zox_log("New Region [%ix%i] Spawned", region_position.x, region_position.y);
                            }
                            // Generate Seed from Terrain Seed
                            lint region_seed = position_seed2(seed->value, region_position);
                            int2 block_position = region_position_to_block_position(region_position, terrain_depth);
                            int2 block_size = (int2) { region_dividor * chunk_length, region_dividor * chunk_length };
                            entity region = spawn_region(world, prefab_region, e, region_seed, region_position, int2_one, block_position, block_size);
                            int2_hashmap_add(regions->value, region_position, region);
                        }
                    }
                }
            }
        }
        zox_sys_query_end();
    }
} zox_sys_end(RegionSpawnSystem);*/
