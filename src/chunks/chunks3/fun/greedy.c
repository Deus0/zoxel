// Greedy meshing of colored meshes (doc/tech/chunks3.md, Greedy meshing; doc/plans/chunks_refactor.md phase 5): a
//  built mesh's faces merged into bigger quads where it draws the same
//  - only uniform faces merge: their 4 corners one colour (palette and ambient occlusion, or the direction's shade)
//      and, skinned, one bone weight; each merged face had its corners' values, so the gpu interpolates the same and
//      the picture doesn't change (a face in a crease or at a joint stays its own)
//  - a direction and a plane at a time, row by row: a face grows along its first axis (gpu_mesh.c's axis_a), then
//      down rows (axis_b) while the whole width matches; the gpu mesher merges in the same order (the same quads)
//  - a merged quad keeps its first face's corner order and winding, its corners moved out to its width and height
//  - a pass over the mesh after it's built: the walkers stay as they are
byte zox_greedy_mesh = 1;

static const byte greedy_axis_u[6] = { 1, 1, 0, 0, 0, 0 };
static const byte greedy_axis_v[6] = { 2, 2, 2, 2, 1, 1 };

typedef struct {
    byte d;
    int plane, u, v;
    int face;
} greedy_face;

static int greedy_face_compare(const void* x, const void* y) {
    const greedy_face* a = x;
    const greedy_face* b = y;
    if (a->d != b->d) return a->d - b->d;
    if (a->plane != b->plane) return a->plane < b->plane ? -1 : 1;
    if (a->v != b->v) return a->v < b->v ? -1 : 1;
    return a->u < b->u ? -1 : a->u > b->u;
}

static inline float greedy_axis(float3 p, byte axis) {
    return axis == 0 ? p.x : axis == 1 ? p.y : p.z;
}

static inline void greedy_set_axis(float3* p, byte axis, float value) {
    if (axis == 0) p->x = value; else if (axis == 1) p->y = value; else p->z = value;
}

static inline byte greedy_colors_same(color_rgb a, color_rgb b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

static inline byte greedy_bones_same(byte4 a, byte4 b) {
    return a.x == b.x && a.y == b.y && a.z == b.z && a.w == b.w;
}

// faces f and g draw the same where they meet: one colour (and bones) at all their corners
static inline byte greedy_same(const MeshColorRGBs* colors, const BoneIndexes* bones, int f, int g) {
    return greedy_colors_same(colors->value[f * 4], colors->value[g * 4]) &&
        (!bones || greedy_bones_same(bones->value[f * 4], bones->value[g * 4]));
}

// a face's 4 corners alike: it may merge
static inline byte greedy_uniform(const MeshColorRGBs* colors, const BoneIndexes* bones, int f) {
    for (byte c = 1; c < 4; c++) {
        if (!greedy_colors_same(colors->value[f * 4], colors->value[f * 4 + c]) ||
            (bones && !greedy_bones_same(bones->value[f * 4], bones->value[f * 4 + c])))
        {
            return 0;
        }
    }
    return 1;
}

// merges a colored mesh's faces in place (4 vertices and 6 indices a face, as mesh_add_face writes them)
//  - origin: the mesh's corner (a voxel's grid starts there), voxel: a voxel's size at the meshed depth
//  - bones: a skeleton mesh's weights (4 a face), NULL none
void mesh_greedy(MeshIndicies* indicies, MeshVertices* vertices, MeshColorRGBs* colors, BoneIndexes* bones,
    float3 origin, float voxel)
{
    const int faces = vertices->length / 4;
    if (!faces || indicies->length != faces * 6 || colors->length != vertices->length ||
        (bones && bones->length != vertices->length) || voxel <= 0)
    {
        return;
    }
    greedy_face* list = zox_malloc(sizeof(greedy_face) * faces);
    byte* kept = zox_malloc(faces);    // copied as it is: not uniform, not a voxel's size
    int listed = 0, span = 0;
    for (int f = 0; f < faces; f++) {
        const float3* v = &vertices->value[f * 4];
        const int* tri = &indicies->value[f * 6];
        const float3 a = float3_subtract(v[tri[1] - f * 4], v[tri[0] - f * 4]);
        const float3 b = float3_subtract(v[tri[2] - f * 4], v[tri[0] - f * 4]);
        const float3 n = float3_cross(a, b);
        const byte axis = fabsf(n.x) > fabsf(n.y) && fabsf(n.x) > fabsf(n.z) ? 0 : (fabsf(n.y) > fabsf(n.z) ? 1 : 2);
        const byte d = axis * 2 + (greedy_axis(n, axis) > 0);
        float3 lower = v[0], upper = v[0];
        for (byte c = 1; c < 4; c++) {
            lower = (float3) { fminf(lower.x, v[c].x), fminf(lower.y, v[c].y), fminf(lower.z, v[c].z) };
            upper = (float3) { fmaxf(upper.x, v[c].x), fmaxf(upper.y, v[c].y), fmaxf(upper.z, v[c].z) };
        }
        const byte u_axis = greedy_axis_u[d], v_axis = greedy_axis_v[d];
        const float width = greedy_axis(upper, u_axis) - greedy_axis(lower, u_axis);
        const float height = greedy_axis(upper, v_axis) - greedy_axis(lower, v_axis);
        kept[f] = fabsf(width - voxel) > voxel * 0.01f || fabsf(height - voxel) > voxel * 0.01f ||
            !greedy_uniform(colors, bones, f);
        if (kept[f]) {
            continue;
        }
        const greedy_face g = { d, (int) lroundf((greedy_axis(lower, axis) - greedy_axis(origin, axis)) / voxel),
            (int) lroundf((greedy_axis(lower, u_axis) - greedy_axis(origin, u_axis)) / voxel),
            (int) lroundf((greedy_axis(lower, v_axis) - greedy_axis(origin, v_axis)) / voxel), f };
        if (g.u < 0 || g.v < 0) {
            kept[f] = 1;
            continue;
        }
        span = int_max(span, int_max(g.u, g.v) + 1);
        list[listed++] = g;
    }
    qsort(list, listed, sizeof(greedy_face), greedy_face_compare);
    // out: the kept faces, then each merged quad
    int* out_indicies = zox_malloc(sizeof(int) * faces * 6);
    float3* out_vertices = zox_malloc(sizeof(float3) * faces * 4);
    color_rgb* out_colors = zox_malloc(sizeof(color_rgb) * faces * 4);
    byte4* out_bones = bones ? zox_malloc(sizeof(byte4) * faces * 4) : NULL;
    int out = 0;
    for (int f = 0; f < faces; f++) {
        if (!kept[f]) {
            continue;
        }
        for (byte i = 0; i < 6; i++) {
            out_indicies[out * 6 + i] = indicies->value[f * 6 + i] - f * 4 + out * 4;
        }
        memcpy(&out_vertices[out * 4], &vertices->value[f * 4], sizeof(float3) * 4);
        memcpy(&out_colors[out * 4], &colors->value[f * 4], sizeof(color_rgb) * 4);
        if (bones) {
            memcpy(&out_bones[out * 4], &bones->value[f * 4], sizeof(byte4) * 4);
        }
        out++;
    }
    int* mask = zox_malloc(sizeof(int) * span * span);
    for (int start = 0; start < listed;) {
        int end = start;
        while (end < listed && list[end].d == list[start].d && list[end].plane == list[start].plane) {
            end++;
        }
        const byte d = list[start].d;
        const byte u_axis = greedy_axis_u[d], v_axis = greedy_axis_v[d];
        for (int i = 0; i < span * span; i++) {
            mask[i] = -1;
        }
        for (int i = start; i < end; i++) {
            mask[list[i].v * span + list[i].u] = list[i].face;
        }
        for (int v = 0; v < span; v++)
        for (int u = 0; u < span; u++) {
            const int f = mask[v * span + u];
            if (f < 0) {
                continue;
            }
            int w = 1;
            while (u + w < span && mask[v * span + u + w] >= 0 && greedy_same(colors, bones, f, mask[v * span + u + w])) {
                w++;
            }
            int h = 1;
            for (byte grows = 1; grows && v + h < span; ) {
                for (int i = 0; i < w && grows; i++) {
                    const int g = mask[(v + h) * span + u + i];
                    grows = g >= 0 && greedy_same(colors, bones, f, g);
                }
                h += grows;
            }
            for (int j = 0; j < h; j++)
            for (int i = 0; i < w; i++) {
                mask[(v + j) * span + u + i] = -1;
            }
            // the first face's corners moved out to the quad's far sides
            const float3* corners = &vertices->value[f * 4];
            float low_u = greedy_axis(corners[0], u_axis), low_v = greedy_axis(corners[0], v_axis);
            for (byte c = 1; c < 4; c++) {
                low_u = fminf(low_u, greedy_axis(corners[c], u_axis));
                low_v = fminf(low_v, greedy_axis(corners[c], v_axis));
            }
            for (byte c = 0; c < 4; c++) {
                float3 p = corners[c];
                if (greedy_axis(p, u_axis) > low_u + voxel * 0.5f) greedy_set_axis(&p, u_axis, low_u + w * voxel);
                if (greedy_axis(p, v_axis) > low_v + voxel * 0.5f) greedy_set_axis(&p, v_axis, low_v + h * voxel);
                out_vertices[out * 4 + c] = p;
                out_colors[out * 4 + c] = colors->value[f * 4 + c];
                if (bones) {
                    out_bones[out * 4 + c] = bones->value[f * 4 + c];
                }
            }
            for (byte i = 0; i < 6; i++) {
                out_indicies[out * 6 + i] = indicies->value[f * 6 + i] - f * 4 + out * 4;
            }
            out++;
        }
        start = end;
    }
    zox_free(mask);
    zox_free(list);
    zox_free(kept);
    zox_free(indicies->value);
    zox_free(vertices->value);
    zox_free(colors->value);
    indicies->value = out_indicies;
    indicies->length = out * 6;
    vertices->value = out_vertices;
    vertices->length = out * 4;
    colors->value = out_colors;
    colors->length = out * 4;
    if (bones) {
        zox_free(bones->value);
        bones->value = out_bones;
        bones->length = out * 4;
    }
}
