entity spawn_poly_terrain(ecs* world, entity parent, lint seed) {
    const int2 cells = poly_terrain_cells;
    const float2 size = poly_terrain_size;
    PolyHeights heights = { 0 };
    spinlock_init(&heights.lock);
    initialize_PolyHeights(&heights, (cells.x + 1) * (cells.y + 1));
    if (!heights.value) {
        return 0;
    }
    poly_height_generate(heights.value, cells, size, seed);
    zox_instance(prefab_poly_terrain);
    zox_setv(e, PolyCells, cells);
    zox_setv(e, Size2, size);
    zox_setv(e, Seed, seed);
    zox_setv(e, Position3D, ((float3) { -size.x / 2, 0, -size.y / 2 }));
    zox_set_ptr(e, PolyHeights, heights);
    dispose_PolyHeights(&heights);
    if (parent) {
        zox_set_parent(world, e, parent);
        if (zox_has(parent, ClockTime)) {
            zox_link(world, e, ClockLink, parent);
        }
    }
    if (zox_valid(material_colored3D_lit)) {
        zox_link(world, e, MaterialLink, material_colored3D_lit);   // lit by the sun, its faces flat shaded
    }
    zox_add(e, Dirty);
    return e;
}
