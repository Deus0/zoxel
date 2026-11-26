static entity zox_tst_poly_terrain;

static byte zox_tst_poly_uploaded(ecs* world, lint seed) {
    if (!zox_tst_poly_terrain) {
        zox_defer_begin();
        zox_tst_poly_terrain = spawn_poly_terrain(world, 0, seed);
        zox_defer_end();
    }
    zox_tst_waiting = "the poly terrain mesh to upload";
    return zox_valid(zox_tst_poly_terrain) && zox_getv(zox_tst_poly_terrain, MeshRenderCount) > 0;
}

static byte zox_tst_poly_mesh_ready(ecs* world) {
    return zox_tst_poly_uploaded(world, 666);
}

static void zox_tst_poly_mesh_check(ecs* world) {
    const entity e = zox_tst_poly_terrain;
    const int2 cells = zox_getv(e, PolyCells);
    const float2 size = zox_getv(e, Size2);
    const PolyHeights* heights = zox_get(e, PolyHeights);
    const MeshVertices* vertices = zox_get(e, MeshVertices);
    const int terrain_count = cells.x * cells.y * 6;
    const int count = terrain_count + 6;
    zox_tst_check(vertices->length == count, "expected %i vertices, got %i", count, vertices->length);
    zox_tst_check(zox_get(e, MeshIndicies)->length == count, "expected %i mesh indices", count);
    zox_tst_check(zox_get(e, MeshColors)->length == count, "expected %i mesh colors", count);
    for (int i = 0; i < terrain_count && i < vertices->length; i++) {
        const float3 p = vertices->value[i];
        const float expected = poly_height_at(heights->value, cells, size, p.x, p.z);
        if (!(fabsf(p.y - expected) < 1e-4f)) {
            zox_tst_check(0, "terrain vertex %i has height %f, expected %f", i, p.y, expected);
            break;
        }
    }
    zox_tst_check(zox_log_errors_count - zox_tst_errors_start == 0, "poly_mesh logged errors");
}
