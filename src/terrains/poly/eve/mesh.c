// its ground colour, unlit: the lit material (colored3D_lit) lights the face
static color_rgb poly_face_color(float3 a, float3 b, float3 c) {
    const float3 normal = float3_normalize(float3_cross(float3_sub(b, a), float3_sub(c, a)));
    return poly_color_at((a.y + b.y + c.y) / 3, normal);
}

static void poly_mesh_write_triangle(float3* vertices, color* colors, float3 a, float3 b, float3 c, color_rgb color) {
    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;
    colors[0] = colors[1] = colors[2] = color_glow(color, 0);
}

static void poly_mesh_write_ground(float3* vertices, color* colors, const float* heights, int2 cells, float2 size) {
    for (int z = 0; z < cells.y; z++) {
        const float z0 = size.y * ((float) z / cells.y);
        const float z1 = size.y * ((float) (z + 1) / cells.y);
        for (int x = 0; x < cells.x; x++) {
            const float x0 = size.x * ((float) x / cells.x);
            const float x1 = size.x * ((float) (x + 1) / cells.x);
            const float3 a = { x0, poly_height_index(heights, cells, x, z), z0 };
            const float3 b = { x1, poly_height_index(heights, cells, x + 1, z), z0 };
            const float3 c = { x1, poly_height_index(heights, cells, x + 1, z + 1), z1 };
            const float3 d = { x0, poly_height_index(heights, cells, x, z + 1), z1 };
            // Same diagonal as poly_height_at, counterclockwise from above.
            poly_mesh_write_triangle(vertices, colors, a, c, b, poly_face_color(a, c, b));
            poly_mesh_write_triangle(vertices + 3, colors + 3, a, d, c, poly_face_color(a, d, c));
            vertices += 6;
            colors += 6;
        }
    }
}

static void poly_mesh_write_sea(float3* vertices, color* colors, float2 size) {
    const float3 a = { -size.x / 2, poly_terrain_sea, -size.y / 2 };
    const float3 b = {  size.x * 1.5f, poly_terrain_sea, a.z };
    const float3 c = { b.x, poly_terrain_sea, size.y * 1.5f };
    const float3 d = { a.x, poly_terrain_sea, c.z };
    const color_rgb water = { 50, 110, 190 };
    poly_mesh_write_triangle(vertices, colors, a, c, b, water);
    poly_mesh_write_triangle(vertices + 3, colors + 3, a, d, c, water);
}

static void build_poly_mesh(ecs* world, entity e, const PolyHeights* heights, int2 cells, float2 size) {
    if (heights->length != (cells.x + 1) * (cells.y + 1)) {
        zox_loge("[poly] height grid does not match cell counts");
        return;
    }
    MeshVertices* vertices = zox_get_mut(e, MeshVertices);
    MeshIndicies* indices = zox_get_mut(e, MeshIndicies);
    MeshColors* colors = zox_get_mut(e, MeshColors);
    const int ground_count = cells.x * cells.y * 6;
    const int count = ground_count + 6;
    resize_MeshVertices(vertices, count);
    resize_MeshIndicies(indices, count);
    resize_MeshColors(colors, count);
    if (vertices->length != count || indices->length != count || colors->length != count) {
        return;
    }
    poly_mesh_write_ground(vertices->value, colors->value, heights->value, cells, size);
    poly_mesh_write_sea(vertices->value + ground_count, colors->value + ground_count, size);
    for (int i = 0; i < count; i++) {
        indices->value[i] = i;
    }
    zox_add(e, MeshDirty);
}

static void poly_mesh_event(iter* it) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PolyHeights);
    zox_sys_in(PolyCells);
    zox_sys_in(Size2);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PolyHeights, heights);
        zox_sys_i(PolyCells, cells);
        zox_sys_i(Size2, size);
        build_poly_mesh(world, e, heights, cells->value, size->value);
    }
}
