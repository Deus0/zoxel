// The terrain's height anywhere (biomes/fun/blend.c), its realm's biomes set up once: the heights system fills
//  tunks' height maps with it, towns ask it where a home stands (a home crossing tunks gets one height in all)
typedef struct {
    lint seed;
    byte terrain_length;
    int max_height;
    float frequencies[zox_biomes_max];
    BiomeClimates climates;
} TerrainHeights;

static inline byte terrain_heights_begin(ecs* world, entity terrain, TerrainHeights* th) {
    const entity realm = zox_valid(terrain) ? zox_get_parent(world, terrain) : 0;
    if (!zox_valid(realm) || !zox_has(realm, BiomeLinks)) {
        return 0;
    }
    const BiomeLinks* realm_biomes = zox_get(realm, BiomeLinks);
    if (!realm_biomes->length) {
        return 0;
    }
    th->seed = zox_getv(terrain, Seed);
    th->terrain_length = octree_size(zox_getv(terrain, NodeDepth));
    th->max_height = int_min(render_distance_y * (th->terrain_length - 1), 255);
    // each biome's height frequency, heights blend between them near their edges
    const byte biomes_count = zox_disable_biomes ? 1 : (byte) int_min(realm_biomes->length, zox_biomes_max);
    for (byte b = 0; b < biomes_count; b++) {
        const entity biome = realm_biomes->value[b];
        th->frequencies[b] = zox_valid(biome) ? zox_getv(biome, BiomeHeightFrequency) : 0.01f;
    }
    th->climates = get_biome_climates(th->terrain_length, th->seed, biomes_count);
    return 1;
}

// the land's height before lakes are carved into it
static inline int terrain_height_base(const TerrainHeights* th, int2 global_position) {
    if (zox_flatlands) {
        return th->max_height / 2;
    }
    return get_terrain_height(global_position, th->terrain_length, th->seed, th->frequencies, &th->climates,
        th->max_height, zox_biome_blend);
}

// Lakes: a cell of zox_lake_cell blocks may hold one, a bowl carved into the land, filled with water up to a block
//  under its rim's lowest point, so it stays in its bowl (doc/tech/water.md)
//  - its centre is seeded, far enough in that the bowl stays in its cell: a column only asks its own cell
//  - one under the sea is only a dip: the sea fills it
#define zox_lake_cell 64
#define zox_lake_chance 30          // percent of cells
#define zox_lake_rim_samples 16
typedef struct {
    int2 centre;
    byte radius;
    byte depth;
} TerrainLake;

static inline uint lake_hash(int x, int y, lint seed, uint salt) {
    uint h = (uint) x * 374761393u + (uint) y * 668265263u + (uint) seed * 2246822519u + salt * 3266489917u;
    h = (h ^ (h >> 13)) * 1274126177u;
    return h ^ (h >> 16);
}

static inline int lake_floor_div(int a, int b) {
    return a >= 0 ? a / b : -((-a + b - 1) / b);
}

// the lake in a column's cell, 0 if none
static inline byte terrain_lake_in_cell(const TerrainHeights* th, int2 global_position, TerrainLake* lake) {
    if (zox_flatlands) {
        return 0;
    }
    const int cx = lake_floor_div(global_position.x, zox_lake_cell), cy = lake_floor_div(global_position.y, zox_lake_cell);
    if (lake_hash(cx, cy, th->seed, 1) % 100 >= zox_lake_chance) {
        return 0;
    }
    lake->radius = 8 + lake_hash(cx, cy, th->seed, 2) % 8;
    lake->depth = 3 + lake_hash(cx, cy, th->seed, 3) % 3;
    const int margin = lake->radius + 2, span = zox_lake_cell - margin * 2;
    lake->centre = (int2) { cx * zox_lake_cell + margin + (int) (lake_hash(cx, cy, th->seed, 4) % span),
        cy * zox_lake_cell + margin + (int) (lake_hash(cx, cy, th->seed, 5) % span) };
    return 1;
}

// how deep a lake's bowl is at a column, 0 outside it
static inline int terrain_lake_dip(const TerrainLake* lake, int2 global_position) {
    const float dx = global_position.x - lake->centre.x, dy = global_position.y - lake->centre.y;
    const float d = sqrtf(dx * dx + dy * dy) / lake->radius;
    return d >= 1 ? 0 : (int) roundf(lake->depth * (1 - d * d));
}

// a lake's water level: a block under its rim's lowest point (the land round it before carving)
static inline int terrain_lake_level(const TerrainHeights* th, const TerrainLake* lake) {
    int lowest = 255;
    for (byte i = 0; i < zox_lake_rim_samples; i++) {
        const float a = i * 2 * (float) M_PI / zox_lake_rim_samples;
        const int2 rim = { lake->centre.x + (int) roundf(cosf(a) * lake->radius), lake->centre.y + (int) roundf(sinf(a) * lake->radius) };
        const int h = terrain_height_base(th, rim);
        lowest = h < lowest ? h : lowest;
    }
    return lowest - 1;
}

static inline int terrain_height_at(const TerrainHeights* th, int2 global_position) {
    const int height = terrain_height_base(th, global_position);
    TerrainLake lake;
    if (!terrain_lake_in_cell(th, global_position, &lake)) {
        return height;
    }
    const int carved = height - terrain_lake_dip(&lake, global_position);
    return carved < 0 ? 0 : carved;
}

// the water level over a column: the sea's, or its lake's if higher and the column is in its bowl, 0 none
static inline int terrain_water_level_at(const TerrainHeights* th, int2 global_position, int sea) {
    TerrainLake lake;
    if (!terrain_lake_in_cell(th, global_position, &lake) || !terrain_lake_dip(&lake, global_position)) {
        return sea;
    }
    const int level = terrain_lake_level(th, &lake);
    return level > sea ? level : sea;
}

// a region's lakes as placement zones (circles, regions/fun/zones.c), so mountains and towns keep out of them
static inline void region_zones_add_lakes(ecs* world, entity terrain, int2 region_position, int2 region_size, RegionZones* zones) {
    TerrainHeights th;
    if (!terrain_heights_begin(world, terrain, &th)) {
        return;
    }
    const int4 cells = {
        lake_floor_div(region_position.x, zox_lake_cell), lake_floor_div(region_position.x + region_size.x, zox_lake_cell),
        lake_floor_div(region_position.y, zox_lake_cell), lake_floor_div(region_position.y + region_size.y, zox_lake_cell) };
    for (int cx = cells.x; cx <= cells.y; cx++) for (int cy = cells.z; cy <= cells.w; cy++) {
        TerrainLake lake;
        if (zones->circles < region_zones_max && terrain_lake_in_cell(&th, (int2) { cx * zox_lake_cell, cy * zox_lake_cell }, &lake)) {
            zones->circle_positions[zones->circles] = lake.centre;
            zones->circle_radii[zones->circles++] = lake.radius + 2;
        }
    }
}
