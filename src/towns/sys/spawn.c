// NOTE: Returns 1 if successfully added a new position/size

byte is_overlap_bounds(int2* positions, byte2* sizes, uint count, int2 position, byte2 size) {
    for (uint i = 0; i < count; i++) {
        int2 p = positions[i];
        byte2 s = sizes[i];
        if (position.x - size.x / 2 < p.x + s.x / 2 &&
            position.x + size.x / 2 > p.x - s.x / 2 &&
            position.y - size.y / 2 < p.y + s.y / 2 &&
            position.y + size.y / 2> p.y - s.y / 2) {
            return 1;
            }
    }
    return 0;
}

// NOTE: Towns now avoid mountains
byte is_overlap_points(int2* positions, byte* radii, uint count, int2 position, byte2 size) {
    int left   = position.x - size.x / 2;
    int right  = position.x + size.x / 2;
    int top    = position.y - size.y / 2;
    int bottom = position.y + size.y / 2;
    for (uint i = 0; i < count; i++) {
        int2 p = positions[i];
        byte r = radii[i];
        if (left   < p.x + r &&
            right  > p.x - r &&
            top    < p.y + r &&
            bottom > p.y - r)
        {
            return 1;
        }
    }
    return 0;
}

byte get_place_position(lint seed, int2 region_position, int2 region_size, byte2 region_padding, byte2 minimum_size, byte2 maximum_size, byte2 place_padding, int2* positions, byte2* sizes, byte added, int2* mountain_positions, byte* mountain_radii, byte mountains_count, uint max_attempts) {
    if (minimum_size.x > maximum_size.x || minimum_size.y > maximum_size.y) {
        zox_loge("Min size greater than max size [get_place_position]");
        return 0;
    }
    int4 region_bounds = (int4) {
        region_position.x + region_padding.x,
        region_position.x + region_size.x - region_padding.x,
        region_position.y + region_padding.y,
        region_position.y + region_size.y - region_padding.y
    };
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        lint seed2 = seed + attempt * 100 + added * 1000;
        byte2 size = {
            seed_range(seed2 + 0, minimum_size.x, maximum_size.x),
            seed_range(seed2 + 1, minimum_size.y, maximum_size.y)
        };
        int2 position = {
            seed_range(seed2 + 2, region_bounds.x + size.x / 2, region_bounds.y - size.x / 2),
            seed_range(seed2 + 3, region_bounds.z + size.y / 2, region_bounds.w - size.y / 2)
        };
        if (is_overlap_points(mountain_positions, mountain_radii, mountains_count, position, size)) {
            continue;
        }
        if (is_overlap_bounds(positions, sizes, added, position, byte2_add(size, place_padding))) {
            continue;
        }
        positions[added] = position;
        sizes[added] = size;
        return 1;
    }
    return 0;
}

// TODO: Add Region Position to Town Positions
// NOTE: Spawns X Towns per region
zox_sys2(RegionTownsSystem) {
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
                zox_log(" - Mountain [%ix%i] Size [%i]", mountain_positions[j].x, mountain_positions[j].y, mountain_radii[j]);
            }
        }
        int2 positions[spawn_count];
        byte2 sizes[spawn_count];
        for (int j = 0; j < spawn_count; j++) {
            if (!get_place_position(seed->value + j * seed_shift, block_position->value, block_size->value, region_padding, min_size, max_size, town_padding, positions, sizes, j, mountain_positions, mountain_radii, mountains_length, max_attempts)) {
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
} zox_sys_end(RegionTownsSystem);
