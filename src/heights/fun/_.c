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

static inline int terrain_height_at(const TerrainHeights* th, int2 global_position) {
    if (zox_flatlands) {
        return th->max_height / 2;
    }
    return get_terrain_height(global_position, th->terrain_length, th->seed, th->frequencies, &th->climates,
        th->max_height, zox_biome_blend);
}
