// tst: region_place, placing things in a region (regions/fun/place.c, mountains, towns, homes, dungeons)
//  - circles (mountains): inside the region, not overlapping, the same seed places the same
//  - rects (towns, homes): inside the region less its padding, not overlapping each other or the circles they avoid
//  - run: bash bsh/tst.sh region_place
#define zox_tst_place_circles 12
#define zox_tst_place_rects 6

static byte zox_tst_place_circles_run(lint seed, int2 region_position, int2 region_size, int2* positions, byte* radii) {
    byte added = 0;
    for (int i = 0; i < zox_tst_place_circles; i++) {
        added += region_place_circle(seed + i * 31, region_position, region_size, 4, 20, positions, radii, added);
    }
    return added;
}

byte zox_tst_region_place_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_region_place_check(ecs* world) {
    (void) world;
    // circles
    const int2 region_position = { 1000, 2000 };
    const int2 region_size = { 256, 256 };
    int2 positions[zox_tst_place_circles];
    byte radii[zox_tst_place_circles];
    const byte circles = zox_tst_place_circles_run(777, region_position, region_size, positions, radii);
    zox_tst_check(circles >= 3, "only [%i] of [%i] circles placed", circles, zox_tst_place_circles);
    int outside = 0, overlaps = 0;
    for (int i = 0; i < circles; i++) {
        const int2 p = positions[i];
        const int r = radii[i];
        outside += p.x - r < region_position.x || p.x + r > region_position.x + region_size.x ||
            p.y - r < region_position.y || p.y + r > region_position.y + region_size.y;
        for (int j = 0; j < i; j++) {
            const int dx = p.x - positions[j].x, dy = p.y - positions[j].y;
            const int sum = r + radii[j];
            overlaps += dx * dx + dy * dy < sum * sum;
        }
    }
    zox_tst_check(!outside, "[%i] circles reach outside the region", outside);
    zox_tst_check(!overlaps, "[%i] pairs of circles overlap", overlaps);
    // the same seed, the same circles
    int2 positions2[zox_tst_place_circles];
    byte radii2[zox_tst_place_circles];
    const byte circles2 = zox_tst_place_circles_run(777, region_position, region_size, positions2, radii2);
    int differs = circles2 != circles;
    for (int i = 0; i < circles && i < circles2; i++) {
        differs += positions[i].x != positions2[i].x || positions[i].y != positions2[i].y || radii[i] != radii2[i];
    }
    zox_tst_check(!differs, "the same seed placed differently, [%i] differences", differs);
    // rects, avoiding two circles
    const int2 rect_region = { 0, 0 };
    const int2 rect_region_size = { 300, 300 };
    const byte2 padding = { 8, 8 };
    int2 avoid_positions[2] = { { 150, 150 }, { 60, 240 } };
    byte avoid_radii[2] = { 30, 20 };
    int2 rect_positions[zox_tst_place_rects];
    byte2 rect_sizes[zox_tst_place_rects];
    byte rects = 0;
    for (int i = 0; i < zox_tst_place_rects; i++) {
        rects += region_place_rect(999 + i * 17, rect_region, rect_region_size, padding, (byte2) { 20, 20 }, (byte2) { 40, 40 },
            (byte2) { 4, 4 }, rect_positions, rect_sizes, rects, avoid_positions, avoid_radii, 2, 200);
    }
    zox_tst_check(rects >= 2, "only [%i] of [%i] rects placed", rects, zox_tst_place_rects);
    int rect_outside = 0, rect_overlaps = 0, on_circles = 0;
    for (int i = 0; i < rects; i++) {
        const int2 p = rect_positions[i];
        const byte2 s = rect_sizes[i];
        rect_outside += p.x - s.x / 2 < rect_region.x + padding.x || p.x + s.x / 2 > rect_region.x + rect_region_size.x - padding.x ||
            p.y - s.y / 2 < rect_region.y + padding.y || p.y + s.y / 2 > rect_region.y + rect_region_size.y - padding.y;
        rect_overlaps += is_overlap_bounds(rect_positions, rect_sizes, i, p, s);
        on_circles += is_overlap_points(avoid_positions, avoid_radii, 2, p, s);
    }
    zox_tst_check(!rect_outside, "[%i] rects reach outside the region's padding", rect_outside);
    zox_tst_check(!rect_overlaps, "[%i] rects overlap others", rect_overlaps);
    zox_tst_check(!on_circles, "[%i] rects overlap the circles they avoid", on_circles);
}

// tst: region_place_prior, rects placed after earlier ones (dungeons after towns) avoid them, and a failed try
//  leaves no gap: only placed rects count (placed, not the try number)
//  - run: bash bsh/tst.sh region_place_prior
void zox_tst_region_place_prior_check(ecs* world) {
    (void) world;
    int2 positions[16];
    byte2 sizes[16];
    // four earlier rects (towns) across the region
    const int2 towns[4] = { { 60, 60 }, { 200, 60 }, { 60, 200 }, { 200, 200 } };
    byte placed = 0;
    for (int i = 0; i < 4; i++) {
        positions[placed] = towns[i];
        sizes[placed++] = (byte2) { 40, 40 };
    }
    const byte prior = placed;
    for (int i = 0; i < 8; i++) {
        if (region_place_rect(777 + i * 13, int2_zero, int2_single(256), byte2_single(4), byte2_single(10),
            byte2_single(20), byte2_single(2), positions, sizes, placed, NULL, NULL, 0, 200)) {
            placed++;
        }
    }
    zox_tst_check(placed > prior, "nothing placed after the earlier rects");
    int overlaps = 0;
    for (int a = prior; a < placed; a++) {
        for (int b = 0; b < a; b++) {
            const int dx = abs(positions[a].x - positions[b].x), dy = abs(positions[a].y - positions[b].y);
            overlaps += dx < (sizes[a].x + sizes[b].x) / 2 && dy < (sizes[a].y + sizes[b].y) / 2;
        }
    }
    zox_log("[tst] [metric] region_place_prior [%i] placed after [%i] earlier, [%i] overlaps", placed - prior, prior, overlaps);
    zox_tst_check(!overlaps, "[%i] rects overlap earlier ones or each other", overlaps);
}

void zox_tsts_regions() {
    zox_tst_add("region_place", zox_tst_region_place_ready, zox_tst_region_place_check, 30);
    zox_tst_add("region_place_prior", zox_tst_region_place_ready, zox_tst_region_place_prior_check, 30);
}
