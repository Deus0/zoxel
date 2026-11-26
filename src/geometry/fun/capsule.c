// a capsule's surface point: around (0 to 1), up (0 bottom to 1 top, over its rings), its normal out
static inline float3 capsule_point(float radius, float half_axis, int segments, int rings, int around, int up, float3* normal) {
    const float a = 2 * M_PI * around / segments;
    // rings over the bottom half sphere (0 to rings), then the top one (rings + 1 to 2 rings + 1)
    const byte top = up > rings;
    const float t = top ? (float) (up - rings - 1) / rings : (float) up / rings;
    const float polar = top ? t * M_PI / 2 : -M_PI / 2 + t * M_PI / 2;
    *normal = (float3) { cosf(polar) * cosf(a), sinf(polar), cosf(polar) * sinf(a) };
    return (float3) { normal->x * radius, normal->y * radius + (top ? half_axis : -half_axis), normal->z * radius };
}

// a capsule mesh (a cylinder with half spheres on its ends), its centre at 0, along y: total height, radius, flat
//  shaded (each triangle its own 3 vertices), lit from above, its front (+z) darker so it shows its facing
void build_capsule_mesh(MeshVertices* vertices, MeshIndicies* indices, MeshColors* colors, float radius,
    float height, int segments, color_rgb tint) {
    const int rings = int_max(segments / 4, 1);
    const int bands = rings * 2 + 1;
    const int count = segments * bands * 6;
    const float half_axis = float_max(height / 2 - radius, 0);
    resize_MeshVertices(vertices, count);
    resize_MeshIndicies(indices, count);
    resize_MeshColors(colors, count);
    int v = 0;
    for (int up = 0; up < bands; up++) {
        for (int around = 0; around < segments; around++) {
            float3 n;
            const float3 a = capsule_point(radius, half_axis, segments, rings, around, up, &n);
            const float3 b = capsule_point(radius, half_axis, segments, rings, around + 1, up, &n);
            const float3 c = capsule_point(radius, half_axis, segments, rings, around + 1, up + 1, &n);
            const float3 d = capsule_point(radius, half_axis, segments, rings, around, up + 1, &n);
            const float3 quad[6] = { a, c, b, a, d, c };
            for (int k = 0; k < 6; k += 3) {
                float3 normal = float3_normalize(float3_cross(float3_sub(quad[k + 1], quad[k]), float3_sub(quad[k + 2], quad[k])));
                const float3 middle = float3_scale(float3_add(float3_add(quad[k], quad[k + 1]), quad[k + 2]), 1.0f / 3);
                float light = 0.45f + 0.55f * fmaxf(0, normal.y * 0.8f + normal.z * 0.2f);
                if (normal.z > 0.7f && middle.y > 0) {
                    light *= 0.55f;
                }
                const color shaded = color_glow((color_rgb) { tint.r * light, tint.g * light, tint.b * light }, 0);
                for (int j = 0; j < 3; j++) {
                    vertices->value[v] = quad[k + j];
                    colors->value[v] = shaded;
                    indices->value[v] = v;
                    v++;
                }
            }
        }
    }
}
