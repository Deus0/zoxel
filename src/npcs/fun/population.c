// Population: how many npcs live in a chunk column, 0 to 255, a map on its region (sys/population.c)
//  - one cell a chunk column over the region's blocks (BlockPosition2, BlockSize2), made once its towns are placed
//  - towns are full, it fades over population_falloff blocks past their walls, to population_base out in the wild
#define population_base 40
#define population_town 255
#define population_falloff 48.0f

static inline byte population_at(int2 global, const int2* town_centers, const byte2* town_sizes, uint towns_count) {
    float density = population_base;
    for (uint i = 0; i < towns_count; i++) {
        // blocks past the town's walls (0 inside it)
        const float dx = fmaxf(0, abs(global.x - town_centers[i].x) - town_sizes[i].x / 2.0f);
        const float dy = fmaxf(0, abs(global.y - town_centers[i].y) - town_sizes[i].y / 2.0f);
        const float past = sqrtf(dx * dx + dy * dy);
        const float near = population_base + (population_town - population_base) * fmaxf(0, 1 - past / population_falloff);
        density = near > density ? near : density;
    }
    return (byte) density;
}

// a region's cells along a side: its blocks over a chunk's
static inline int2 population_map_size(int2 region_blocks, int chunk_length) {
    return (int2) { region_blocks.x / chunk_length, region_blocks.y / chunk_length };
}

// a chunk column's population (its block x and z), from its region's map, -1 if the map isn't made yet
static inline int region_population_at(ecs* world, entity region, int2 column_block, int chunk_length) {
    if (!zox_valid(region) || !zox_has(region, PopulationMap)) {
        return -1;
    }
    const PopulationMap* map = zox_get(region, PopulationMap);
    const int2 start = zox_getv(region, BlockPosition2);
    const int2 size = population_map_size(zox_getv(region, BlockSize2), chunk_length);
    const int2 cell = { (column_block.x - start.x) / chunk_length, (column_block.y - start.y) / chunk_length };
    if (!map->length || map->length != size.x * size.y || cell.x < 0 || cell.y < 0 || cell.x >= size.x || cell.y >= size.y) {
        return -1;
    }
    return map->value[cell.x + cell.y * size.x];
}
