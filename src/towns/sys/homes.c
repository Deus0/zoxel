// TODO: Add Region Position to Town Positions
// NOTE: Spawns X Towns per region
zox_sys2(HomesSpawnSystem) {
    byte dbg_log = 0;
    uint max_attempts = 100;
    // byte2 towns_count = (byte2) { 1, 9 };
    byte min_homes_count = 2;
    byte max_homes_count = 6;
    // byte2 min_size = (byte2) { 48, 48 };
    // byte2 max_size = (byte2) { 128, 128 };
    // byte2 wall_height_range = (byte2) { 2, 6 };
    // byte2 wall_thickness_range = (byte2) { 1, 4 };
    byte2 padding = byte2_single(8);
    // byte2 town_padding = byte2_single(12);
    byte2 home_min_size = (byte2) { 5, 5 };
    byte2 home_max_size = (byte2) { 12, 12 };
    byte2 home_padding = byte2_single(4);
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Seed);
    zox_sys_in(BlockPosition2);
    zox_sys_in(TownSize);
    zox_sys_out(GenerateTown);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(Seed, seed);
        zox_sys_i(BlockPosition2, position);
        zox_sys_i(TownSize, size);
        zox_sys_o(GenerateTown, generate);
        // Waits a frame for parent to set on mountains
        if (!generate->value) {
            continue;
        }
        // byte spawn_count = seed_range(seed->value, towns_count.x, towns_count.y);
        /*entity region = zox_get_parent(world, e);
        if (!zox_valid(region)) {
            continue;
        }*/
        /*entity terrain = zox_get_parent(world, region);
        if (!zox_valid(terrain)) {
            continue;
        }*/
        if (dbg_log) {
            zox_log("   - Town Position [%ix%i]", position->value.x, position->value.y);
        };
        // Spawn Homes
        // Have to corner the position for our finder function
        int2 town_size2 = byte2_to_int2(size->value);
        int2 town_position2 = int2_sub(position->value, int2_half(town_size2));
        byte spawn_count = rand_range(min_homes_count, max_homes_count);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Homes", zox_get_name(e), spawn_count);
        };
        int2 home_positions[spawn_count];
        byte2 home_sizes[spawn_count];
        for (int k = 0; k < spawn_count; k++) {
            if (!get_place_position(seed->value, town_position2, town_size2, padding, home_min_size, home_max_size, home_padding, home_positions, home_sizes, k, NULL, NULL, 0, max_attempts)) {
                continue;
            }
            int2 home_position = home_positions[k];
            byte2 home_size = home_sizes[k];
            lint home_seed = position_seed2(seed->value, home_position);
            spawn_home(world, prefab_home, e, home_seed, home_position, home_size, 4);
            if (dbg_log) {
                zox_log("   + Home [%ix%i] Size [%ix%i]", home_position.x, home_position.y, home_size.x, home_size.y);
            }
        }
        generate->value = 0;
    }
} zox_sys_end(HomesSpawnSystem);
