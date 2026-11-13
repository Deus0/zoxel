// Placement zones in one place: every area placed in a region is a child tagged PlacementZone, a circle (Radius)
//  or a rect (ZoneSize), at its BlockPosition2. A placer finds them all, places new ones apart from them, spawns.
//  - region_zones_find: a region's zones, whoever placed them (mountains, towns, dungeons)
//  - region_zones_place_circle / _rect: a new zone apart from all of them, added so the next avoids it too
//    (spawns are deferred, the new entities aren't children until the merge)
//  - the shapes and checks are place.c's, tst region_place, region_place_prior, region_zones
#define region_zones_max 64

typedef struct {
    int2 circle_positions[region_zones_max];
    byte circle_radii[region_zones_max];
    byte circles;
    int2 rect_positions[region_zones_max];
    byte2 rect_sizes[region_zones_max];
    byte rects;
} RegionZones;

void region_zones_find(ecs* world, entity region, RegionZones* zones) {
    zones->circles = 0;
    zones->rects = 0;
    entity children[zox_children_capacity];
    const uint count = zox_get_children_by_id(world, region, children, zox_children_capacity, zox_id(PlacementZone));
    for (uint i = 0; i < count; i++) {
        const entity e = children[i];
        const int2 position = zox_getv(e, BlockPosition2);
        if (zox_has(e, Radius) && zones->circles < region_zones_max) {
            zones->circle_positions[zones->circles] = position;
            zones->circle_radii[zones->circles++] = zox_getv(e, Radius);
        } else if (zox_has(e, ZoneSize) && zones->rects < region_zones_max) {
            zones->rect_positions[zones->rects] = position;
            zones->rect_sizes[zones->rects++] = zox_getv(e, ZoneSize);
        }
    }
}

// a circle zone apart from the other circles (circles are placed first, mountains), 1 if placed
byte region_zones_place_circle(RegionZones* zones, lint seed, int2 region_position, int2 region_size,
    byte minimum_radius, byte maximum_radius, int2* position, byte* radius)
{
    if (zones->circles >= region_zones_max ||
        !region_place_circle(seed, region_position, region_size, minimum_radius, maximum_radius,
            zones->circle_positions, zones->circle_radii, zones->circles)) {
        return 0;
    }
    *position = zones->circle_positions[zones->circles];
    *radius = zones->circle_radii[zones->circles++];
    return 1;
}

// a rect zone apart from every rect and circle, 1 if placed
byte region_zones_place_rect(RegionZones* zones, lint seed, int2 region_position, int2 region_size, byte2 region_padding,
    byte2 minimum_size, byte2 maximum_size, byte2 place_padding, uint max_attempts, int2* position, byte2* size)
{
    if (zones->rects >= region_zones_max ||
        !region_place_rect(seed, region_position, region_size, region_padding, minimum_size, maximum_size, place_padding,
            zones->rect_positions, zones->rect_sizes, zones->rects,
            zones->circle_positions, zones->circle_radii, zones->circles, max_attempts)) {
        return 0;
    }
    *position = zones->rect_positions[zones->rects];
    *size = zones->rect_sizes[zones->rects++];
    return 1;
}
