// TODO: Add Region Position to Town Positions
// NOTE: Spawns X Towns per region
void homes_spawn_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    uint max_attempts = 100;
    byte min_homes_count = 1;
    byte max_homes_count = 12;
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
        // Waits a frame for parent to set on mountains; only its homes' step (its roads' step is sys/roads.c's)
        if (generate->value != 1) {
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
        // from its seed, the same town has the same homes (zox_rand() gave a different count each time)
        byte spawn_count = seed_range(seed->value * 7349, min_homes_count, max_homes_count);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Homes", zox_get_name(e), spawn_count);
        };
        // each home's floor: the terrain at its middle (its region's terrain)
        const entity region = zox_get_parent(world, e);
        TerrainHeights th;
        if (!terrain_heights_begin(world, zox_valid(region) ? zox_get_parent(world, region) : 0, &th)) {
            zox_loge("[%s] can't place homes, no terrain heights", zox_get_name(e));
            generate->value = 0;
            continue;
        }
        // placed: only placed homes count, a failed try left a gap the next ones checked against
        // its square first, in its middle, homes placed round it (fun/town.c)
        int2 home_positions[spawn_count + 1];
        byte2 home_sizes[spawn_count + 1];
        home_positions[0] = position->value;
        home_sizes[0] = byte2_single(town_square_half * 2 + 1);
        // clear of its walls, room for a road round them (8 left homes in walls up to 12 thick); its walls curve
        //  (fun/town.c), a home is kept if its corners are a block inside them, tried again elsewhere if not
        const byte2 padding = byte2_single(zox_getv(e, WallThickness) + 3);
        const TownShape shape = town_shape_of(world, e);
        byte placed = 1;
        for (int k = 0; k < spawn_count * 3 && placed <= spawn_count; k++) {
            if (!region_place_rect(seed->value + k * 6151, town_position2, town_size2, padding, home_min_size, home_max_size, home_padding, home_positions, home_sizes, placed, NULL, NULL, 0, max_attempts)) {
                continue;
            }
            const int2 home_position = home_positions[placed];
            const byte2 home_size = home_sizes[placed];
            byte inside = 1;
            for (byte c = 0; c < 4 && inside; c++) {
                inside = town_inside(&shape, (int2) { home_position.x + (c & 1 ? 1 : -1) * (home_size.x / 2 + 1),
                    home_position.y + (c & 2 ? 1 : -1) * (home_size.y / 2 + 1) }, 1);
            }
            if (!inside) {
                continue;
            }
            placed++;
            lint home_seed = position_seed2(seed->value, home_position);
            const byte floor = (byte) int_clamp(terrain_height_at(&th, home_position), 0, 255);
            spawn_home(world, prefab_home, e, home_seed, home_position, home_size, floor, home_facing_to(home_position, position->value));
            if (dbg_log) {
                zox_log("   + Home [%ix%i] Size [%ix%i]", home_position.x, home_position.y, home_size.x, home_size.y);
            }
        }
        // its roads next, once its homes are in (sys/roads.c)
        generate->value = town_generate_roads;
    }
    zox_sys_on_end();
} zoxd_system(homes_spawn_system);
