float poly_height_index(const float* heights, int2 cells, int x, int z) {
    x = int_clamp(x, 0, cells.x);
    z = int_clamp(z, 0, cells.y);
    return heights[x + (size_t) z * (cells.x + 1)];
}

// Height and rise in each axis across the selected triangle.
typedef struct {
    float height;
    float rise_x;
    float rise_z;
} poly_triangle_sample;

static inline poly_triangle_sample poly_sample_triangle(
    const float* heights, int2 cells, float2 size, float x, float z
) {
    const float gx = clampf(x / size.x, 0, 1) * cells.x;
    const float gz = clampf(z / size.y, 0, 1) * cells.y;
    const int i = int_min((int) gx, cells.x - 1);
    const int j = int_min((int) gz, cells.y - 1);
    const float fx = gx - i;
    const float fz = gz - j;
    const float h00 = poly_height_index(heights, cells, i, j);
    const float h11 = poly_height_index(heights, cells, i + 1, j + 1);
    float rise_x, rise_z;
    if (fx >= fz) {
        const float h10 = poly_height_index(heights, cells, i + 1, j);
        rise_x = h10 - h00;
        rise_z = h11 - h10;
    } else {
        const float h01 = poly_height_index(heights, cells, i, j + 1);
        rise_x = h11 - h01;
        rise_z = h01 - h00;
    }
    return (poly_triangle_sample) { h00 + fx * rise_x + fz * rise_z, rise_x, rise_z };
}

float poly_height_at(const float* heights, int2 cells, float2 size, float x, float z) {
    return poly_sample_triangle(heights, cells, size, x, z).height;
}

float3 poly_normal_at(const float* heights, int2 cells, float2 size, float x, float z) {
    const poly_triangle_sample sample = poly_sample_triangle(heights, cells, size, x, z);
    const float dx = sample.rise_x / (size.x / cells.x);
    const float dz = sample.rise_z / (size.y / cells.y);
    return float3_normalize((float3) { -dx, 1, -dz });
}

// an island: noise hills stretched to use their whole range, a mountain in the middle, a round falloff
//  to the sea (the edges always under it); the same seed makes the same heights
//  - perlin_noise's hash only shifts its gradients a little by seed, so the seed also moves where the noise is read
//    from (mixed, splitmix64), else every seed draws about the same island
void poly_height_generate(float* heights, int2 cells, float2 size, lint seed) {
    uint64_t mix = (uint64_t) seed + 0x9E3779B97F4A7C15ull;
    mix = (mix ^ (mix >> 30)) * 0xBF58476D1CE4E5B9ull;
    mix = (mix ^ (mix >> 27)) * 0x94D049BB133111EBull;
    mix ^= mix >> 31;
    const float offset_x = (float) (mix & 0xffff) * 0.37f;
    const float offset_z = (float) ((mix >> 16) & 0xffff) * 0.37f;
    for (int z = 0; z <= cells.y; z++) {
        const float pz = size.y * ((float) z / cells.y);
        const float dz = (pz - size.y * 0.5f) / (size.y * 0.5f);
        for (int x = 0; x <= cells.x; x++) {
            const float px = size.x * ((float) x / cells.x);
            const float dx = (px - size.x * 0.5f) / (size.x * 0.5f);
            const float d = sqrtf(dx * dx + dz * dz);     // 0 in the middle, 1 at the edges' middles
            float e = clampf((1 - d) / 0.35f, 0, 1);       // land inside 0.65, sea past 1
            e = e * e * (3 - 2 * e);
            const double noise = perlin_octaves(px + offset_x, pz + offset_z, poly_terrain_frequency, (uint) seed, poly_terrain_octaves);
            const float hills = clampf(((float) noise - 0.25f) / 0.5f, 0, 1);
            float peak = clampf(1 - d / 0.5f, 0, 1);          // a mountain in the middle half
            peak = peak * peak * (3 - 2 * peak);
            const float n = clampf(hills * 0.35f + peak * 0.75f, 0, 1);
            const float height = poly_terrain_height * powf(n, 1.3f) * e;
            heights[x + (size_t) z * (cells.x + 1)] = float_lerp(-2, height, e);
        }
    }
}

color_rgb poly_color_at(float height, float3 normal) {
    if (height < 0) {
        return (color_rgb) { 155, 140, 105 };
    }
    if (height < poly_terrain_sea + 0.5f) {
        return (color_rgb) { 220, 205, 150 };
    }
    if (normal.y < 0.75f) {
        return (color_rgb) { 120, 115, 110 };
    }
    if (height > 0.8f * poly_terrain_height) {
        return (color_rgb) { 240, 240, 245 };
    }
    return (color_rgb) { 90, 160, 70 };
}
