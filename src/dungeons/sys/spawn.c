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
        entity mountains[zox_children_capacity];
        uint mountains_length = zox_get_children_by_id(world, e, mountains, zox_children_capacity, zox_id(Radius));
        if (dbg_log) {
            zox_log("Region has [%i] Mountains", mountains_length);
        }
        int2 mountain_positions[mountains_length];
        byte mountain_radii[mountains_length];
        for (int j = 0; j < mountains_length; j++) {
            entity e2 = mountains[j];
            mountain_positions[j] = zox_getv(e2, BlockPosition2);
            mountain_radii[j] = zox_getv(e2, Radius);
            if (dbg_log) {
                zox_log(" - Mountain [%ix%i] Size [%i]",
                    mountain_positions[j].x,
                    mountain_positions[j].y,
                    mountain_radii[j]);
            }
        }
        // the region's towns first, prior rects a dungeon avoids, then each dungeon placed
        entity towns[zox_children_capacity];
        const uint towns_count = zox_get_children_by_id(world, e, towns, zox_children_capacity, zox_id(Town));
        int2 positions[towns_count + spawn_count];
        byte2 sizes[towns_count + spawn_count];
        byte placed = 0;
        for (uint j = 0; j < towns_count; j++) {
            positions[placed] = zox_getv(towns[j], BlockPosition2);
            sizes[placed++] = zox_getv(towns[j], TownSize);
        }
        for (int j = 0; j < spawn_count; j++) {
            if (!region_place_rect(
                seed->value,
                block_position->value,
                block_size->value,
                margins,
                min_size,
                max_size,
                padding,
                positions,
                sizes,
                placed,
                mountain_positions,
                mountain_radii,
                mountains_length,
                max_attempts))
            {
                continue;
            }
            int2 position = positions[placed];
            byte2 size = sizes[placed++];
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
