// Homes: one place for a home, its ground levelled with steps to its door (town_map_system), its blocks a column at a
//  time (town_walls_system)
//  - its ground: one floor height (the terrain at its middle, terrain_height_at, the same in every tunk it crosses),
//    the heightmap under it and a block round it set to it: the terrain fills up under it (a foundation) or is dug
//    away, steps down (or up) to the ground in front of its door
//  - its roof: flat, or peaked (rising a block a column in from its long sides to a ridge, gable walls at its ends),
//    in its biome's wood, a glow stone set into its middle (lights inside, shows outside)
#define town_homes_max 64
#define home_height 6
#define home_door_width 2
#define home_steps 6        // columns in front of its door stepping to the ground

typedef struct {
    int2 center;
    byte2 half;     // its size / 2, its walls at center +- half
    byte style;     // 0 flat, 1 peaked
} TownHome;

typedef struct {
    byte wall;
    byte floor;
    byte roof;      // its biome's wood
    byte glow;      // a glow stone in the roof, 0 for none
} HomeBlocks;

static inline TownHome town_home(ecs* world, entity home) {
    const byte2 size = zox_getv(home, HomeSize);
    return (TownHome) { zox_getv(home, BlockPosition2), { size.x / 2, size.y / 2 }, zox_getv(home, HomeStyle) };
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

// the home a column is in, NULL if none
static inline const TownHome* town_home_at(const TownHome* homes, byte count, int2 g) {
    for (byte i = 0; i < count; i++) {
        const TownHome* h = &homes[i];
        if (abs(g.x - h->center.x) <= h->half.x && abs(g.y - h->center.y) <= h->half.y) {
            return h;
        }
    }
    return NULL;
}

// what a column of a home is: its door (the middle of its front, +z), a wall or inside
static inline byte home_part_at(const TownHome* h, int2 g) {
    const byte front = g.y == h->center.y + h->half.y;
    if (front && abs(g.x - h->center.x) <= home_door_width / 2) {
        return zox_town_type_home_door;
    }
    if (front || g.y == h->center.y - h->half.y || abs(g.x - h->center.x) == h->half.x) {
        return zox_town_type_home_wall;
    }
    return zox_town_type_home;
}

// its ground in a column: the floor under it and a block round it, steps in front of its door, -1 if it leaves it
//  - natural: the terrain's height there
static inline int home_ground_at(const TownHome* h, int floor, int2 g, int natural) {
    if (abs(g.x - h->center.x) <= h->half.x + 1 && abs(g.y - h->center.y) <= h->half.y + 1) {
        return floor;
    }
    const int out = g.y - (h->center.y + h->half.y);
    if (abs(g.x - h->center.x) > home_door_width / 2 || out < 1 || out > home_steps) {
        return -1;
    }
    // a block a column towards the ground, from the margin (at its floor) outside its door
    const int step = out - 1;
    if (natural < floor - step) {
        return floor - step;
    }
    if (natural > floor + step) {
        return floor + step;
    }
    return natural;
}

// blocks a roof rises over its walls at a column: 0 flat, peaked a block a column in from its long sides
static inline int home_roof_rise(const TownHome* h, int2 g) {
    if (!h->style) {
        return 0;
    }
    // the ridge runs along its longer side, it slopes across the shorter
    const byte along_x = h->half.x >= h->half.y;
    const int across = along_x ? g.y : g.x;
    const int center = along_x ? h->center.y : h->center.x;
    const int half = along_x ? h->half.y : h->half.x;
    const int in_low = across - (center - half), in_high = (center + half) - across;
    return in_low < in_high ? in_low : in_high;
}

// a peaked roof's end walls, filled up to the roof (a triangle)
static inline byte home_is_gable(const TownHome* h, int2 g) {
    if (!h->style) {
        return 0;
    }
    return h->half.x >= h->half.y ? abs(g.x - h->center.x) == h->half.x : abs(g.y - h->center.y) == h->half.y;
}

// sets a block in a chunk at a world height, if it's in this chunk
static inline byte town_set_block(VoxelNode* voctree, byte depth, byte3* position, int global_y, int chunk_y, byte hmultiplier, byte length, byte value) {
    if (global_y < chunk_y) {
        return 0;
    }
    const int y = (global_y - chunk_y) / hmultiplier;
    if (y >= length) {
        return 0;
    }
    position->y = y;
    set_clean_VoxelNode(voctree, depth, *position, value);
    return 1;
}

// a home's blocks in a column of a chunk, its ground at height: its floor, walls, the lintel over its door, its roof,
//  1 if it set its glow stone (at position)
static inline byte home_build_column(VoxelNode* voctree, byte depth, byte3* position, int chunk_y, byte hmultiplier,
    byte length, const TownHome* h, const HomeBlocks* blocks, int2 g, int height)
{
    const byte part = home_part_at(h, g);
    town_set_block(voctree, depth, position, height + 1, chunk_y, hmultiplier, length, blocks->floor);
    const int wall_from = part == zox_town_type_home_wall ? 2 : part == zox_town_type_home_door ? home_height - 1 : home_height;
    for (int y = wall_from; y < home_height; y++) {
        town_set_block(voctree, depth, position, height + y, chunk_y, hmultiplier, length, blocks->wall);
    }
    const int roof_y = height + home_height + home_roof_rise(h, g);
    if (home_is_gable(h, g)) {
        for (int y = height + home_height; y < roof_y; y++) {
            town_set_block(voctree, depth, position, y, chunk_y, hmultiplier, length, blocks->wall);
        }
    }
    const byte is_glow = blocks->glow && int2_equals(g, h->center);
    return town_set_block(voctree, depth, position, roof_y, chunk_y, hmultiplier, length, is_glow ? blocks->glow : blocks->roof) && is_glow;
}
