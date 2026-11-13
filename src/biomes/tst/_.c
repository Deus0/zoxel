// tst: biome_heights, terrain heights blend between biomes, no raises where biomes meet
//  - walks zox_tst_biome_columns columns in a line, through many biome edges, with the presets' frequencies
//  - each step's height change at an edge (the biome changes) vs inside a biome
//  - blended (zox_biome_blend): the biggest step at an edge is no bigger than inside biomes (+1 rounding)
//  - unblended (blend 0) logged to compare, the raises it had
//  - run: bash bsh/tst.sh biome_heights
// tst: biome_map, a column's biome is in range, every biome is somewhere, the same seed gives the same biomes
//  - run: bash bsh/tst.sh biome_map
#define zox_tst_biome_columns 40000
#define zox_tst_biome_length 16
#define zox_tst_biome_seed 4242
static const float zox_tst_biome_frequencies[] = { 0.022216f, 0.03125f, 0.015625f, 0.017212f, 0.014832f, 0.01125f, 0.018181f, 0.009999f };
#define zox_tst_biomes_count 8

typedef struct {
    int edges;
    int edge_step;      // biggest height change stepping over a biome edge
    int inner_step;     // biggest inside a biome
    double edge_mean;
    double inner_mean;
} ZoxTstBiomeSteps;

static ZoxTstBiomeSteps zox_tst_biome_steps(float blend) {
    ZoxTstBiomeSteps steps = { 0 };
    long edge_total = 0, inner_total = 0;
    int inner = 0;
    int last_height = -1;
    byte last_biome = 0;
    for (int x = 0; x < zox_tst_biome_columns; x++) {
        const int2 position = (int2) { x, 777 };
        const byte biome = get_biome_index(get_biome_value(position, zox_tst_biome_length, zox_tst_biome_seed, zox_tst_biomes_count));
        const int height = get_terrain_height(position, zox_tst_biome_length, zox_tst_biome_seed,
            zox_tst_biome_frequencies, zox_tst_biomes_count, 255, blend);
        if (last_height >= 0) {
            const int step = abs(height - last_height);
            if (biome != last_biome) {
                steps.edges++;
                edge_total += step;
                if (step > steps.edge_step) steps.edge_step = step;
            } else {
                inner++;
                inner_total += step;
                if (step > steps.inner_step) steps.inner_step = step;
            }
        }
        last_height = height;
        last_biome = biome;
    }
    steps.edge_mean = steps.edges ? (double) edge_total / steps.edges : 0;
    steps.inner_mean = inner ? (double) inner_total / inner : 0;
    return steps;
}

byte zox_tst_biomes_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_biome_heights_check(ecs* world) {
    (void) world;
    const ZoxTstBiomeSteps blended = zox_tst_biome_steps(zox_biome_blend);
    const ZoxTstBiomeSteps unblended = zox_tst_biome_steps(0);
    zox_log("[tst] [metric] biome_heights blended: edges [%i] biggest step at an edge [%i] (mean %.2f), inside [%i] (mean %.2f)",
        blended.edges, blended.edge_step, blended.edge_mean, blended.inner_step, blended.inner_mean);
    zox_log("[tst] [metric] biome_heights unblended: biggest step at an edge [%i] (mean %.2f), inside [%i] (mean %.2f)",
        unblended.edge_step, unblended.edge_mean, unblended.inner_step, unblended.inner_mean);
    zox_tst_check(blended.edges >= 10, "only [%i] biome edges in [%i] columns, the walk should cross many", blended.edges, zox_tst_biome_columns);
    zox_tst_check(blended.edge_step <= blended.inner_step + 1,
        "a step of [%i] where biomes meet, inside biomes the biggest is [%i]: a raise", blended.edge_step, blended.inner_step);
    // blending only changes columns near edges: blend 0 is the unblended height
    const int2 middle = (int2) { 123, 456 };
    const double value = get_biome_value(middle, zox_tst_biome_length, zox_tst_biome_seed, zox_tst_biomes_count);
    const byte biome = get_biome_index(value);
    const int expected = (int) floorf(get_biome_height(middle, zox_tst_biome_length, zox_tst_biome_seed, zox_tst_biome_frequencies[biome]));
    const int unblended_height = get_terrain_height(middle, zox_tst_biome_length, zox_tst_biome_seed,
        zox_tst_biome_frequencies, zox_tst_biomes_count, 255, 0);
    zox_tst_check(unblended_height == (expected < 0 ? 0 : (expected > 255 ? 255 : expected)),
        "unblended height [%i], its biome's height is [%i]", unblended_height, expected);
}

void zox_tst_biome_map_check(ecs* world) {
    (void) world;
    int counts[zox_tst_biomes_count] = { 0 };
    int out_of_range = 0, differs = 0;
    for (int x = 0; x < 600; x++) {
        for (int y = 0; y < 600; y++) {
            const int2 position = (int2) { x * 7, y * 7 };
            const double value = get_biome_value(position, zox_tst_biome_length, zox_tst_biome_seed, zox_tst_biomes_count);
            const byte biome = get_biome_index(value);
            if (value < 0 || biome >= zox_tst_biomes_count) {
                out_of_range++;
                continue;
            }
            counts[biome]++;
            // the same seed and column, the same biome
            differs += get_biome_index(get_biome_value(position, zox_tst_biome_length, zox_tst_biome_seed, zox_tst_biomes_count)) != biome;
        }
    }
    zox_tst_check(!out_of_range, "[%i] columns had a biome out of range", out_of_range);
    zox_tst_check(!differs, "[%i] columns got a different biome the second time", differs);
    int missing = 0;
    for (int b = 0; b < zox_tst_biomes_count; b++) {
        missing += !counts[b];
    }
    zox_log("[tst] [metric] biome_map columns a biome: %i %i %i %i %i %i %i %i", counts[0], counts[1], counts[2], counts[3],
        counts[4], counts[5], counts[6], counts[7]);
    zox_tst_check(!missing, "[%i] of [%i] biomes never appear", missing, zox_tst_biomes_count);
}

void zox_tsts_biomes() {
    zox_tst_add("biome_heights", zox_tst_biomes_ready, zox_tst_biome_heights_check, 30);
    zox_tst_add("biome_map", zox_tst_biomes_ready, zox_tst_biome_map_check, 30);
}
