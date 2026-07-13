void add_debug_cube(ecs* world, entity e, float3 size) {
    zox_prefab_set(e, DebugCubeLines, { 1 });
    zox_add_tag(e, CubeLines);
    zox_prefab_set(e, LineThickness, { 4 });
    zox_prefab_set(e, ColorRGB, {{ 0, 255, 255 }});
    zox_prefab_set(e, Bounds3D, { size });
    zox_prefab_set(e, RenderDepth, { 0 });
    zox_prefab_set(e, RenderDisabled, { 0 });
    zox_prefab_set(e, MeshIndicies, { 0 });
    zox_prefab_set(e, MesnRenderCount, { 0 });
}

/*void debug_cubec(
    ecs* world,
    const float3 p,
    const float3 s,
    color_rgb c
) {
    // Calculate the min and max corners
    float3 min = {
        p.x - s.x * 0.5f,
        p.y - s.y * 0.5f,
        p.z - s.z * 0.5f
    };
    float3 max = {
        p.x + s.x * 0.5f,
        p.y + s.y * 0.5f,
        p.z + s.z * 0.5f
    };

    // Bottom square
    float3 a = {min.x, min.y, min.z};
    float3 b = {max.x, min.y, min.z};
    float3 c_ = {max.x, min.y, max.z};
    float3 d = {min.x, min.y, max.z};

    // Top square
    float3 e = {min.x, max.y, min.z};
    float3 f = {max.x, max.y, min.z};
    float3 g = {max.x, max.y, max.z};
    float3 h = {min.x, max.y, max.z};

    // Bottom edges
    debug_linec(world, a, b, c);
    debug_linec(world, b, c_, c);
    debug_linec(world, c_, d, c);
    debug_linec(world, d, a, c);

    // Top edges
    debug_linec(world, e, f, c);
    debug_linec(world, f, g, c);
    debug_linec(world, g, h, c);
    debug_linec(world, h, e, c);

    // Vertical edges
    debug_linec(world, a, e, c);
    debug_linec(world, b, f, c);
    debug_linec(world, c_, g, c);
    debug_linec(world, d, h, c);
}*/

static inline void debug_cubec(ecs* world, float3 p, float3 s, color_rgb line_color, float thickness) {
    // Half size
    const float hx = s.x * 0.5f;
    const float hy = s.y * 0.5f;
    const float hz = s.z * 0.5f;
    // Corner positions
    float3 v[8] = {
        { p.x - hx, p.y - hy, p.z - hz }, // 0
        { p.x + hx, p.y - hy, p.z - hz }, // 1
        { p.x + hx, p.y - hy, p.z + hz }, // 2
        { p.x - hx, p.y - hy, p.z + hz }, // 3
        { p.x - hx, p.y + hy, p.z - hz }, // 4
        { p.x + hx, p.y + hy, p.z - hz }, // 5
        { p.x + hx, p.y + hy, p.z + hz }, // 6
        { p.x - hx, p.y + hy, p.z + hz }  // 7
    };
    // Edge index pairs
    static const byte edges[12][2] = {
        {0,1}, {1,2}, {2,3}, {3,0}, // bottom
        {4,5}, {5,6}, {6,7}, {7,4}, // top
        {0,4}, {1,5}, {2,6}, {3,7}  // verticals
    };
    for (int i = 0; i < 12; i++) {
        // debug_linec(world, v[edges[i][0]], v[edges[i][1]], col);
        spawn_line3_thickness(world, v[edges[i][0]], v[edges[i][1]], line_color, thickness);
    }
}
