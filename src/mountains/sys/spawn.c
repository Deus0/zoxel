
// NOTE: Spawns X Towns per region
void region_mountain_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    byte min_size = 24;
    byte max_size = 128;
    byte min_height = 3;
    byte max_height = 16;
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
        if (generate->value != zox_generate_region_mountains) {
            continue;
        }
        byte spawn_count = seed_range(seed->value, 1, 9);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Mountains", zox_get_name(e), spawn_count);
        }
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        // the region's placement zones, each mountain a circle apart from them (regions/fun/zones.c)
        RegionZones zones;
        region_zones_find(world, e, &zones);
        for (int j = 0; j < spawn_count; j++) {
            int2 mountain_position;
            byte radius;
            if (!region_zones_place_circle(&zones, seed->value, block_position->value, block_size->value, min_size, max_size,
                &mountain_position, &radius)) {
                continue;
            }
            byte mountain_chance = seed_range(seed->value * 36963, 0, 100);
            byte mountain_type = zox_mountain_type_hill;
            if (mountain_chance >= 90) {
                mountain_type = zox_mountain_type_peak;
            }
            byte height = seed_range(seed->value + j, min_height, max_height);
            // mountain_position = int2_add(mountain_position, spawn_position);
            lint mountain_seed = position_seed2(seed->value, mountain_position);
            spawn_mountain(world, prefab_mountain, e, mountain_seed, mountain_type, mountain_position, radius, height);
            if (dbg_log) {
                zox_log("   ++ Mountain [%ix%i] H [%i] R [%i]", mountain_position.x, mountain_position.y, height, radius);
            }
        }
        generate->value = zox_generate_region_towns_trigger;
    }
    zox_sys_on_end();
} zoxd_system(region_mountain_system);
