// tst: poly_heights, triangle sampling and deterministic island generation
static byte zox_tst_poly_heights_ready(ecs* world) {
    (void) world;
    return 1;
}

static void zox_tst_poly_heights_check(ecs* world) {
    (void) world;
    const float epsilon = 1e-4f;
    const int2 cells = { 2, 2 };
    const float2 size = { 2, 2 };
    const float plane[] = { 0, 1, 2, 1, 2, 3, 2, 3, 4 };
    const float peak[] = { 0, 1, 2, 2, 5, 3, 2, 3, 4 };
    const float flat[9] = { 0 };
    for (int z = 0; z <= cells.y; z++) {
        for (int x = 0; x <= cells.x; x++) {
            const int index = x + z * (cells.x + 1);
            zox_tst_check(fabsf(poly_height_at(plane, cells, size, x, z) - plane[index]) < epsilon,
                "plane vertex [%i, %i] has the wrong height", x, z);
            zox_tst_check(fabsf(poly_height_at(peak, cells, size, x, z) - peak[index]) < epsilon,
                "peak vertex [%i, %i] has the wrong height", x, z);
        }
    }
    const float3 slope = float3_normalize((float3) { -1, 1, -1 });
    const float3 up = { 0, 1, 0 };
    for (int z = 0; z <= 8; z++) {
        for (int x = 0; x <= 8; x++) {
            const float px = x * 0.25f;
            const float pz = z * 0.25f;
            zox_tst_check(fabsf(poly_height_at(plane, cells, size, px, pz) - (px + pz)) < epsilon,
                "plane sample [%f, %f] has the wrong height", px, pz);
            zox_tst_check(float3_length(float3_sub(poly_normal_at(plane, cells, size, px, pz), slope)) < epsilon,
                "plane sample [%f, %f] has the wrong normal", px, pz);
            zox_tst_check(float3_length(float3_sub(poly_normal_at(flat, cells, size, px, pz), up)) < epsilon,
                "flat sample [%f, %f] should point up", px, pz);
        }
    }
    // First triangle: 0 + 0.75 * (1 - 0) + 0.25 * (5 - 1) = 1.75.
    zox_tst_check(fabsf(poly_height_at(peak, cells, size, 0.75f, 0.25f) - 1.75f) < epsilon,
        "peak height on the first triangle should be 1.75");
    // Second triangle: 0 + 0.25 * (5 - 2) + 0.75 * (2 - 0) = 2.25.
    zox_tst_check(fabsf(poly_height_at(peak, cells, size, 0.25f, 0.75f) - 2.25f) < epsilon,
        "peak height on the second triangle should be 2.25");
    zox_tst_check(fabsf(poly_height_index(peak, cells, -2, 3) - peak[6]) < epsilon,
        "out-of-bounds vertex should clamp to the corner");
    zox_tst_check(fabsf(poly_height_at(plane, cells, size, 3, 0.25f) - 2.25f) < epsilon,
        "out-of-bounds sample should clamp to the edge");

    enum { side = 129 };
    const int2 island_cells = { side - 1, side - 1 };
    const float2 island_size = { 128, 128 };
    float island[side * side], other[side * side];
    poly_height_generate(island, island_cells, island_size, 666);
    poly_height_generate(other, island_cells, island_size, 666);
    zox_tst_check(memcmp(island, other, sizeof(island)) == 0, "same seed should repeat heights");
    poly_height_generate(other, island_cells, island_size, 667);
    zox_tst_check(memcmp(island, other, sizeof(island)) != 0, "different seed should change heights");
    float highest = island[0];
    for (int z = 0; z < side; z++) {
        for (int x = 0; x < side; x++) {
            const float height = island[x + z * side];
            if (x == 0 || z == 0 || x == side - 1 || z == side - 1) {
                zox_tst_check(height < poly_terrain_sea, "edge [%i, %i] should be below sea", x, z);
            }
            highest = fmaxf(highest, height);
        }
    }
    zox_tst_check(highest > poly_terrain_sea && highest <= poly_terrain_height + epsilon,
        "highest point [%f] should be above sea and at most [%f]", highest, poly_terrain_height);
}

#include "mesh.c"
#include "views.c"

void zox_tsts_poly_terrains(void) {
    zox_tst_add("poly_heights", zox_tst_poly_heights_ready, zox_tst_poly_heights_check, 30);
    zox_tst_set_setup("poly_heights", NULL, 1);
    zox_tst_add("poly_mesh", zox_tst_poly_mesh_ready, zox_tst_poly_mesh_check, 30);
    zox_tst_set_setup("poly_mesh", NULL, 1);
    zox_tst_add("poly_views", zox_tst_poly_views_ready, zox_tst_poly_views_check, 30);
    zox_tst_set_setup("poly_views", NULL, 1);
}
