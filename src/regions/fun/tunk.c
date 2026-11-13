// A tunk's view of its region, for the maps that draw placed things into tunks (mountains, towns, homes)
//  - region_tunk_begin: its region and sizes, the same setup each map system did
//  - region_tunk_box: the map columns a thing's box covers in this tunk, so a thing only touches its own
//    columns (it scanned every column of every tunk in its region)
//  - a thing crosses tunks: each tunk draws its part, tst town_maps checks walls join up across them
typedef struct {
    entity region;
    byte terrain_length;    // blocks along a chunk
    byte length;            // map columns along the tunk at its lod
    int2 map_size;
    byte depth_difference;  // blocks between map columns
    int2 global_start;      // the tunk's first column, in blocks
} RegionTunk;

// 0 if the tunk has no region yet, or its region is still placing things
static inline byte region_tunk_begin(ecs* world, entity tunk, byte lod, int2 tunk_position, RegionTunk* t) {
    t->region = zox_get_link(world, tunk, RegionLink);
    if (!zox_valid(t->region) || zox_getv(t->region, GenerateRegion)) {
        return 0;
    }
    const entity terrain = zox_get_parent(world, tunk);
    const byte terrain_depth = zox_getv(terrain, NodeDepth);
    t->terrain_length = octree_size(terrain_depth);
    t->length = octree_size(lod);
    t->map_size = int2_single(t->length);
    t->depth_difference = octree_size(terrain_depth - lod);
    t->global_start = (int2) { tunk_position.x * t->terrain_length, tunk_position.y * t->terrain_length };
    return 1;
}

// a map column's position in blocks
static inline int2 region_tunk_global(const RegionTunk* t, int2 position) {
    return (int2) {
        t->global_start.x + position.x * t->depth_difference,
        t->global_start.y + position.y * t->depth_difference
    };
}

// the map columns (from, to, inclusive) whose blocks are in center - half to center + half, 0 if none
static inline byte region_tunk_box(const RegionTunk* t, int2 center, int2 half, int2* from, int2* to) {
    const int dd = t->depth_difference;
    const int2 low = (int2) { center.x - half.x - t->global_start.x, center.y - half.y - t->global_start.y };
    const int2 high = (int2) { center.x + half.x - t->global_start.x, center.y + half.y - t->global_start.y };
    // first column at or after low, last at or before high
    from->x = low.x <= 0 ? 0 : (low.x + dd - 1) / dd;
    from->y = low.y <= 0 ? 0 : (low.y + dd - 1) / dd;
    to->x = high.x < 0 ? -1 : high.x / dd;
    to->y = high.y < 0 ? -1 : high.y / dd;
    if (to->x > t->length - 1) to->x = t->length - 1;
    if (to->y > t->length - 1) to->y = t->length - 1;
    return from->x <= to->x && from->y <= to->y;
}
