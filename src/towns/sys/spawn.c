// NOTE: Returns 1 if successfully added a new position/size


// TODO: Add Region Position to Town Positions
// NOTE: Spawns X Towns per region
void region_towns_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    uint max_attempts = 100;
    int seed_shift = 369;
    byte2 towns_count = (byte2) { 1, 9 };
    byte2 min_size = (byte2) { 64, 64 };
    byte2 max_size = (byte2) { 160, 160 };
    byte2 wall_height_range = (byte2) { 5, 12 };
    byte2 wall_thickness_range = (byte2) { 5, 12 };
    byte2 region_padding = byte2_single(8);
    byte2 town_padding = byte2_single(12);
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
        // Waits a frame for parent to set on mountains
        if (generate->value == zox_generate_region_towns_trigger) {
            generate->value = zox_generate_region_towns;
            continue;
        }
        if (generate->value == zox_generate_region_towns_homes) {
            generate->value = zox_generate_region_dungeons;
            continue;
        }
        if (generate->value != zox_generate_region_towns) {
            continue;
        }
        byte spawn_count = seed_range(seed->value, towns_count.x, towns_count.y);
        if (dbg_log) {
            zox_log("[%s] Is Spawning [%i] Towns", zox_get_name(e), spawn_count);
        };
        entity terrain = zox_get_parent(world, e);
        if (!zox_valid(terrain)) {
            continue;
        }
        if (dbg_log) {
            zox_log("   - Region Position [%ix%i]",
                block_position->value.x,
                block_position->value.y);
        };
        entity mountains[zox_children_capacity];
        uint mountains_length = zox_get_children_by_id(
            world,
            e,
            mountains,
            zox_children_capacity,
            zox_id(Radius));
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
                zox_log(" - Mountain [%ix%i] Size [%i]", mountain_positions[j].x, mountain_positions[j].y, mountain_radii[j]);
            }
        }
        int2 positions[spawn_count];
        byte2 sizes[spawn_count];
        for (int j = 0; j < spawn_count; j++) {
            if (!region_place_rect(seed->value + j * seed_shift, block_position->value, block_size->value, region_padding, min_size, max_size, town_padding, positions, sizes, j, mountain_positions, mountain_radii, mountains_length, max_attempts)) {
                continue;
            }
            int2 town_position = positions[j];
            byte2 town_size = sizes[j];
            lint town_seed = position_seed2(seed->value, town_position);
            byte wall_height = seed_range(town_seed + seed_shift * 2, wall_height_range.x, wall_height_range.y);
            byte wall_thickness = seed_range(town_seed + seed_shift * 3, wall_thickness_range.x, wall_thickness_range.y);
            spawn_town(world, prefab_town, e, town_seed, town_position, town_size, wall_height, wall_thickness);
            if (dbg_log) {
                zox_log(" + Town [%ix%i] Size [%ix%i]", town_position.x, town_position.y, town_size.x, town_size.y);
            }
        }
        generate->value = zox_generate_region_towns_homes;
    }
    zox_sys_on_end();
} zoxd_system(region_towns_system);
