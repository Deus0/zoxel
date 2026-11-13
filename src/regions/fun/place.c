// Placement zones: the areas things claim in a region (mountains, towns, homes, dungeons): seeded, inside bounds,
//  not overlapping earlier zones, then each tunk's map draws the parts that fall in it (regions/fun/tunk.c)
//  - region_place_circle: a circle zone, apart from other circles (mountains)
//  - region_place_rect: a rect zone, apart from earlier rects and circles (towns avoid mountains, dungeons avoid
//    towns and mountains, homes)
//  - added counts only zones placed, a failed try leaves no gap (placers keep a placed count)
//  - the same seed places the same, tst region_place, region_place_prior
// NOTE: Returns 1 if successfully added a new position/radius
byte region_place_circle(lint seed, int2 region_position, int2 region_size, byte minimum_radius, byte maximum_radius, int2* positions, byte* radii, byte added) {
    const int max_attempts = 100;
    for (int attempt = 0; attempt < max_attempts; attempt++) {
        lint seed2 = seed + attempt * 100;
        byte new_radius = seed_range(seed2 + 0, minimum_radius, maximum_radius);
        int2 new_position = {
            seed_range(seed2 + 1,
                region_position.x + new_radius,
                region_position.x + region_size.x - new_radius),
            seed_range(seed2 + 2,
                region_position.y + new_radius,
                region_position.y + region_size.y - new_radius)
        };
        byte valid = 1;
        for (int i = 0; i < added; i++) {
            int2 p = positions[i];
            int r = radii[i];
            int dx = new_position.x - p.x;
            int dy = new_position.y - p.y;
            int min_distance = new_radius + r;
            if (dx * dx + dy * dy < min_distance * min_distance) {
                valid = 0;
                break;
            }
        }
        if (valid) {
            positions[added] = new_position;
            radii[added] = new_radius;
            return 1;
        }
    }
    return 0;
}

byte is_overlap_bounds(
    int2* positions,
    byte2* sizes,
    uint count,
    int2 position,
    byte2 size)
{
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
byte is_overlap_points(
    int2* positions,
    byte* radii,
    uint count,
    int2 position,
    byte2 size)
{
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

byte region_place_rect(
    lint seed,
    int2 region_position,
    int2 region_size,
    byte2 region_padding,
    byte2 minimum_size,
    byte2 maximum_size,
    byte2 place_padding,
    int2* positions,
    byte2* sizes,
    byte added,
    int2* mountain_positions,
    byte* mountain_radii,
    byte mountains_count,
    uint max_attempts)
{
    if (minimum_size.x > maximum_size.x ||
        minimum_size.y > maximum_size.y)
    {
        zox_loge("Min size greater than max size [region_place_rect]");
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
