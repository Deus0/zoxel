// NOTE: Spawns Dungeons per Region
void dungeons_spawn_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    uint max_attempts = 100;
    byte2 spawn_counts = (byte2) { 1, 9 };
    byte2 min_size = (byte2) { 48, 48 };
    byte2 max_size = (byte2) { 128, 128 };
    byte2 margins = byte2_single(8);
    byte2 padding = byte2_single(12);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(BlockPosition2);
    zox_sys_in(BlockSize2);
    zox_sys_out(GenerateRegion);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(BlockPosition2, block_position);
        zox_sys_i(BlockSize2, block_size);
        zox_sys_o(GenerateRegion, generate);
        if (generate->value != zox_generate_region_dungeons) {
            continue;
        }
        byte spawn_count = seed_range(seed->value, spawn_counts.x, spawn_counts.y);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Towns", zox_get_name(e), spawn_count);
        };
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        if (dbg_log) {
            zox_log("   - Region Position [%ix%i]", block_position->value.x, block_position->value.y);
        };
        // the region's placement zones (mountains, towns, earlier dungeons), each dungeon a rect apart from them
        RegionZones zones;
        region_zones_find(world, e, &zones);
        for (int j = 0; j < spawn_count; j++) {
            int2 position;
            byte2 size;
            if (!region_zones_place_rect(&zones, seed->value + j * 7919, block_position->value, block_size->value,
                margins, min_size, max_size, padding, max_attempts, &position, &size)) {
                continue;
            }
            lint dungeon_seed = position_seed2(seed->value, position);
            spawn_dungeon(
                world,
                prefab_dungeon,
                e,
                dungeon_seed,
                position,
                size);
            if (dbg_log) {
                zox_log(" + Dungeon [%ix%i] Size [%ix%i]", position.x, position.y, size.x, size.y);
            }
        }
        generate->value = zox_generate_region_population;
    }
    zox_sys_on_end();
} zoxd_system(dungeons_spawn_system);
