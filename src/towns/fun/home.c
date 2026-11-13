// Homes: one place for a home: its ground levelled with steps to its door (town_map_system), its blocks as boxes
//  (build_home, filled into chunks by town_walls_system)
//  - its ground: its floor (HomeFloor, the terrain at its middle), the heightmap under it and a block round it set to
//    it: the terrain fills up under it (a foundation) or is dug away, steps from its door down (or up) to the ground
//  - it faces its door to its town's middle (HomeFacing), written in its own frame (fun/build.c): x across, z out of
//    its front
//  - its roof: flat, or peaked (rising a block a column in from its long sides to a ridge, gable walls at its ends),
//    in its biome's wood, a glow stone set into its middle (lights inside, shows outside)
#define town_homes_max 64
#define home_door_width 2
#define home_steps 6        // columns in front of its door stepping to the ground

typedef struct {
    int2 center;
    byte2 half;     // its size / 2 in the world (x, z), its walls at center +- half
    byte style;     // 0 flat, 1 peaked
    byte floor;
    byte facing;
    byte height;    // its walls, its roof on top
} TownHome;

typedef struct {
    byte wall;
    byte floor;
    byte roof;      // its biome's wood
    byte glow;      // a glow stone in the roof, 0 for none
} HomeBlocks;

static inline TownHome town_home(ecs* world, entity home) {
    const byte2 size = zox_getv(home, HomeSize);
    return (TownHome) { zox_getv(home, BlockPosition2), { size.x / 2, size.y / 2 }, zox_getv(home, HomeStyle),
        zox_getv(home, HomeFloor), zox_getv(home, HomeFacing), zox_getv(home, HomeHeight) };
}

// its half size in its own frame (x across, z out of its front)
static inline int2 home_half_local(const TownHome* h) {
    return h->facing & 1 ? (int2) { h->half.y, h->half.x } : (int2) { h->half.x, h->half.y };
}

// the homes of a chunk's region whose columns cross the chunk (x from, to inclusive, z too)
static inline byte town_homes_in(ecs* world, entity region, int2 from, int2 to, TownHome* homes) {
    byte count = 0;
    entity towns[zox_children_capacity];
    const uint towns_count = zox_valid(region) ? zox_get_children_by_id(world, region, towns, zox_children_capacity, zox_id(Town)) : 0;
    for (uint t = 0; t < towns_count; t++) {
        entity children[zox_children_capacity];
        const uint homes_count = zox_get_children_by_id(world, towns[t], children, zox_children_capacity, zox_id(Home));
        for (uint h = 0; h < homes_count && count < town_homes_max; h++) {
            const TownHome home = town_home(world, children[h]);
            if (home.center.x + home.half.x < from.x || home.center.x - home.half.x > to.x ||
                home.center.y + home.half.y < from.y || home.center.y - home.half.y > to.y) {
                continue;
            }
            homes[count++] = home;
        }
    }
    return count;
}

// its ground in a column: its floor under it and a block round it, steps in front of its door, -1 if it leaves it
//  - natural: the terrain's height there
static inline int home_ground_at(const TownHome* h, int2 g, int natural) {
    const int2 local = build_unturn(h->facing, int2_sub(g, h->center));
    const int2 half = home_half_local(h);
    if (abs(local.x) <= half.x + 1 && abs(local.y) <= half.y + 1) {
        return h->floor;
    }
    const int out = local.y - half.y;
    if (abs(local.x) > home_door_width / 2 || out < 1 || out > home_steps) {
        return -1;
    }
    // a block a column towards the ground, from the margin (at its floor) outside its door
    const int step = out - 1;
    if (natural < h->floor - step) {
        return h->floor - step;
    }
    if (natural > h->floor + step) {
        return h->floor + step;
    }
    return natural;
}

// a home as boxes: its floor, walls, doorway, roof and glow stone, glow set to its glow stone (world blocks)
static inline void build_home(Building* b, const TownHome* h, const HomeBlocks* blocks, int3* glow) {
    *b = (Building) { .center = h->center, .facing = h->facing, .floor = h->floor };
    const int2 half = home_half_local(h);
    const int x = half.x, z = half.y, top = h->height;
    // its floor, a block over its ground
    build_box(b, (int3) { -x, 1, -z }, (int3) { x, 1, z }, blocks->floor);
    // its walls
    build_box(b, (int3) { -x, 2, -z }, (int3) { x, top - 1, -z }, blocks->wall);
    build_box(b, (int3) { -x, 2, z }, (int3) { x, top - 1, z }, blocks->wall);
    build_box(b, (int3) { -x, 2, -z }, (int3) { -x, top - 1, z }, blocks->wall);
    build_box(b, (int3) { x, 2, -z }, (int3) { x, top - 1, z }, blocks->wall);
    // its doorway, in the middle of its front, a lintel over it
    build_box(b, (int3) { -home_door_width / 2, 2, z }, (int3) { home_door_width / 2, top - 2, z }, 0);
    // its roof: flat, or layers rising to a ridge along its longer side, gables filling its ends
    int rise = 0;
    if (!h->style) {
        build_box(b, (int3) { -x, top, -z }, (int3) { x, top, z }, blocks->roof);
    } else if (x >= z) {
        rise = z;
        for (int i = 0; i <= z; i++) {
            build_box(b, (int3) { -x, top + i, -z + i }, (int3) { x, top + i, -z + i }, blocks->roof);
            build_box(b, (int3) { -x, top + i, z - i }, (int3) { x, top + i, z - i }, blocks->roof);
            if (-z + i + 1 <= z - i - 1) {
                build_box(b, (int3) { -x, top + i, -z + i + 1 }, (int3) { -x, top + i, z - i - 1 }, blocks->wall);
                build_box(b, (int3) { x, top + i, -z + i + 1 }, (int3) { x, top + i, z - i - 1 }, blocks->wall);
            }
        }
    } else {
        rise = x;
        for (int i = 0; i <= x; i++) {
            build_box(b, (int3) { -x + i, top + i, -z }, (int3) { -x + i, top + i, z }, blocks->roof);
            build_box(b, (int3) { x - i, top + i, -z }, (int3) { x - i, top + i, z }, blocks->roof);
            if (-x + i + 1 <= x - i - 1) {
                build_box(b, (int3) { -x + i + 1, top + i, -z }, (int3) { x - i - 1, top + i, -z }, blocks->wall);
                build_box(b, (int3) { -x + i + 1, top + i, z }, (int3) { x - i - 1, top + i, z }, blocks->wall);
            }
        }
    }
    // its glow stone, set into the middle of its roof (lights the room under it, shows outside)
    *glow = (int3) { h->center.x, h->floor + top + rise, h->center.y };
    if (blocks->glow) {
        build_box(b, (int3) { 0, top + rise, 0 }, (int3) { 0, top + rise, 0 }, blocks->glow);
    }
}
