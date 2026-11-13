// Biomes and the height of the terrain, as functions of a column's position (used by biome_map_system,
//  height_map_system and tst biome_heights)
//  - a column's biome comes from noise: value = noise * biomes_count, its biome is floor(value)
//  - each biome has its own height noise frequency, so heights jumped where biomes met (raises)
//  - blended: within zox_biome_blend of a biome's edge (in value), the heights of the biomes either side mix
//    smoothly, at the edge itself both sides are the average, so there's no step
#define zox_biome_frequency 0.02
#define zox_biome_octaves 4
#define zox_terrain_octaves 8
#define zox_terrain_height_multiplier 3
float zox_biome_blend = 0.25f;
float zox_biome_contrast = 2.5f;   // spreads the biome noise, all biomes appear (1 was 3 of 8 never)  // a biome's width is 1 in value, this much each side of an edge blends

// the column's biome value, its biome is (int) of it
static inline double get_biome_value(int2 global_position, byte terrain_length, lint seed, byte biomes_count) {
    const double perlin_x = noise_positiver2 + (global_position.x / ((float) terrain_length));
    const double perlin_y = noise_positiver2 + (global_position.y / ((float) terrain_length));
    // the noise clusters around its middle, spread out so the first and last biomes appear too (tst biome_map)
    double noise = 0.5 + (perlin_octaves(perlin_x, perlin_y, zox_biome_frequency, seed, zox_biome_octaves) - 0.5) * zox_biome_contrast;
    noise = noise < 0 ? 0 : noise;
    const double value = noise * biomes_count;
    // the last biome includes the top (noise of 1)
    return value < biomes_count ? value : biomes_count - 0.0001;
}

static inline byte get_biome_index(double biome_value) {
    return biome_value > 0 ? (byte) biome_value : 0;
}

// one biome's height at a column, unclamped
static inline float get_biome_height(int2 global_position, byte terrain_length, lint seed, float frequency) {
    const double perlin_x = noise_positiver2 + (global_position.x / ((float) terrain_length));
    const double perlin_y = noise_positiver2 + (global_position.y / ((float) terrain_length));
    const double perlin_value = perlin_octaves(perlin_x, perlin_y, frequency, seed, zox_terrain_octaves);
    return (float) (perlin_value * terrain_length * zox_terrain_height_multiplier);
}

static inline float smoothstepf(float t) {
    t = t < 0 ? 0 : (t > 1 ? 1 : t);
    return t * t * (3 - 2 * t);
}

// the terrain's height at a column, blending biomes near their edges (blend 0 for the old steps)
//  - frequencies: each biome's BiomeHeightFrequency
static inline int get_terrain_height(
    int2 global_position,
    byte terrain_length,
    lint seed,
    const float* frequencies,
    byte biomes_count,
    int max_height,
    float blend)
{
    float height;
    if (!biomes_count) {
        height = 0;
    } else if (biomes_count == 1) {
        height = get_biome_height(global_position, terrain_length, seed, frequencies[0]);
    } else {
        const double value = get_biome_value(global_position, terrain_length, seed, biomes_count);
        const byte biome = get_biome_index(value);
        const float fraction = (float) (value - biome);
        height = get_biome_height(global_position, terrain_length, seed, frequencies[biome]);
        if (blend > 0 && fraction < blend && biome > 0) {
            // near the edge with the biome below
            const float below = get_biome_height(global_position, terrain_length, seed, frequencies[biome - 1]);
            height = below + (height - below) * smoothstepf((fraction + blend) / (2 * blend));
        } else if (blend > 0 && fraction > 1 - blend && biome + 1 < biomes_count) {
            // near the edge with the biome above
            const float above = get_biome_height(global_position, terrain_length, seed, frequencies[biome + 1]);
            height = height + (above - height) * smoothstepf((fraction - (1 - blend)) / (2 * blend));
        }
    }
    const int value = (int) floorf(height);
    return value < 0 ? 0 : (value > max_height ? max_height : value);
}
