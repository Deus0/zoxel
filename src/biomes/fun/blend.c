// Biomes and the height of the terrain, as functions of a column's position (biome_map_system,
//  height_map_system, tst biome_heights and biome_map)
//  - climate: two noises, temperature and moisture, put each column at a point in climate space
//  - each realm biome has a center in climate space, a column's biome is the nearest center (a patchwork where
//    any biomes can meet, not a line where biome 3 only ever touches 2 and 4)
//  - centers are the climates at seeded spots in the world, so they sit where climates really are and every
//    biome gets a share, spread apart by picking the farthest of a few candidates
//  - heights blend: each biome near a column weighs by how much farther its center is than the nearest,
//    where two meet they weigh the same, so heights have no step there (they had raises, tst biome_heights)
#define zox_biome_climate_frequency 0.02
#define zox_biome_climate_octaves 4
#define zox_biome_center_candidates 8
#define zox_terrain_octaves 8
#define zox_terrain_height_multiplier 3
#define zox_biomes_max 32
float zox_biome_blend = 0.035f;     // climate distance a blend fades over, 0 for hard edges

typedef struct {
    float2 centers[zox_biomes_max];
    byte count;
} BiomeClimates;

// a column's climate: temperature and moisture, 0 to 1
static inline float2 get_biome_climate(int2 global_position, byte terrain_length, lint seed) {
    const double x = noise_positiver2 + (global_position.x / ((float) terrain_length));
    const double y = noise_positiver2 + (global_position.y / ((float) terrain_length));
    return (float2) {
        (float) perlin_octaves(x, y, zox_biome_climate_frequency, seed, zox_biome_climate_octaves),
        (float) perlin_octaves(x, y, zox_biome_climate_frequency, seed + 7919, zox_biome_climate_octaves)
    };
}

static inline float biome_climate_distance(float2 a, float2 b) {
    const float dx = a.x - b.x, dy = a.y - b.y;
    return sqrtf(dx * dx + dy * dy);
}

// a realm's biome centers from its seed: the climates at seeded spots, each the candidate farthest from those
//  already placed
static inline BiomeClimates get_biome_climates(byte terrain_length, lint seed, byte biomes_count) {
    BiomeClimates climates = { .count = biomes_count < zox_biomes_max ? biomes_count : zox_biomes_max };
    uint state = (uint) seed * 2654435761u + 1013904223u;
    for (byte b = 0; b < climates.count; b++) {
        float best_distance = -1;
        for (byte c = 0; c < zox_biome_center_candidates; c++) {
            state = state * 1664525u + 1013904223u;
            const int x = (int) (state % 200000) - 100000;
            state = state * 1664525u + 1013904223u;
            const int y = (int) (state % 200000) - 100000;
            const float2 climate = get_biome_climate((int2) { x, y }, terrain_length, seed);
            float nearest = 10;
            for (byte o = 0; o < b; o++) {
                const float d = biome_climate_distance(climate, climates.centers[o]);
                nearest = d < nearest ? d : nearest;
            }
            if (nearest > best_distance) {
                best_distance = nearest;
                climates.centers[b] = climate;
            }
        }
    }
    return climates;
}

// a climate's biome: the nearest center
static inline byte get_biome_index_at(const BiomeClimates* climates, float2 climate) {
    byte nearest = 0;
    float nearest_distance = 10;
    for (byte b = 0; b < climates->count; b++) {
        const float d = biome_climate_distance(climate, climates->centers[b]);
        if (d < nearest_distance) {
            nearest_distance = d;
            nearest = b;
        }
    }
    return nearest;
}

static inline byte get_biome_index(int2 global_position, byte terrain_length, lint seed, const BiomeClimates* climates) {
    return get_biome_index_at(climates, get_biome_climate(global_position, terrain_length, seed));
}

// one biome's height at a column, unclamped
static inline float get_biome_height(int2 global_position, byte terrain_length, lint seed, float frequency) {
    const double perlin_x = noise_positiver2 + (global_position.x / ((float) terrain_length));
    const double perlin_y = noise_positiver2 + (global_position.y / ((float) terrain_length));
    const double perlin_value = perlin_octaves(perlin_x, perlin_y, frequency, seed, zox_terrain_octaves);
    return (float) (perlin_value * terrain_length * zox_terrain_height_multiplier);
}

// the terrain's height at a column, the biomes near it blended by their weights (blend 0: the nearest only)
//  - frequencies: each biome's BiomeHeightFrequency
static inline int get_terrain_height(
    int2 global_position,
    byte terrain_length,
    lint seed,
    const float* frequencies,
    const BiomeClimates* climates,
    int max_height,
    float blend)
{
    float height = 0;
    if (climates->count == 1) {
        height = get_biome_height(global_position, terrain_length, seed, frequencies[0]);
    } else if (climates->count) {
        const float2 climate = get_biome_climate(global_position, terrain_length, seed);
        float distances[zox_biomes_max];
        float nearest = 10;
        byte nearest_biome = 0;
        for (byte b = 0; b < climates->count; b++) {
            distances[b] = biome_climate_distance(climate, climates->centers[b]);
            if (distances[b] < nearest) {
                nearest = distances[b];
                nearest_biome = b;
            }
        }
        if (blend <= 0) {
            height = get_biome_height(global_position, terrain_length, seed, frequencies[nearest_biome]);
        } else {
            // weights fade to 0 at 4 blends farther than the nearest, so only biomes close by cost a height
            const float cutoff = expf(-4.0f);
            float weights = 0;
            for (byte b = 0; b < climates->count; b++) {
                const float weight = expf(-(distances[b] - nearest) / blend) - cutoff;
                if (weight <= 0) {
                    continue;
                }
                height += weight * get_biome_height(global_position, terrain_length, seed, frequencies[b]);
                weights += weight;
            }
            height /= weights;
        }
    }
    const int value = (int) floorf(height);
    return value < 0 ? 0 : (value > max_height ? max_height : value);
}
