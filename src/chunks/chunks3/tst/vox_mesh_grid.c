// tst: vox_mesh_grid, a vox's mesh read from its pyramid is the same mesh as read from its octree (build_voxel_mesh's
//  two lookups, one walk): the same faces, positions and ao colors, in the same order
//  - a ball of voxels in 3 colors with holes in it, reduced so solid areas are big nodes
//  - every lod: lower lods have nodes with children at their depth, solid on a side only if all children are
//  - split (models) and not split (npc skeletons: uniform nodes are one big voxel)
//  - run: bash bsh/tst.sh vox_mesh_grid

#define zox_tst_vox_grid_depth 4

typedef struct {
    float3 vertices[4];
    color_rgb colors[4];
} zox_tst_vox_face;

static int zox_tst_vox_face_compare(const void* a, const void* b) {
    return memcmp(a, b, sizeof(zox_tst_vox_face));
}

// faces of a built mesh, sorted, so meshes compare without their order
static zox_tst_vox_face* zox_tst_vox_faces(mesh_colored_build_data* mesh, int* count) {
    *count = mesh->vertices->size / 4;
    zox_tst_vox_face* faces = calloc(*count ? *count : 1, sizeof(zox_tst_vox_face));
    for (int i = 0; i < *count; i++) {
        for (byte j = 0; j < 4; j++) {
            faces[i].vertices[j] = mesh->vertices->data[i * 4 + j];
            faces[i].colors[j] = mesh->colors->data[i * 4 + j];
        }
    }
    qsort(faces, *count, sizeof(zox_tst_vox_face), zox_tst_vox_face_compare);
    return faces;
}

static mesh_colored_build_data zox_tst_vox_mesh_new() {
    return (mesh_colored_build_data) {
        .indicies = create_int_array_d(initial_dynamic_array_size),
        .vertices = create_float3_array_d(initial_dynamic_array_size),
        .colors = create_color_rgb_array_d(initial_dynamic_array_size)
    };
}

static void zox_tst_vox_mesh_dispose(mesh_colored_build_data* mesh) {
    dispose_int_array_d(mesh->indicies);
    dispose_float3_array_d(mesh->vertices);
    dispose_color_rgb_array_d(mesh->colors);
}

static void zox_tst_vox_mesh_grid_compare(const VoxelNode* vox, const ColorRGBs* colors, byte node_depth, byte depth) {
    const float scale = 2.0f;
    const float3 offset = (float3) { -1, -1, -1 };
    mesh_colored_build_data octree_mesh = zox_tst_vox_mesh_new();
    build_voxel_mesh(vox, NULL, colors, &octree_mesh, node_depth, depth, offset, scale, 0);
    mesh_colored_build_data grid_mesh = zox_tst_vox_mesh_new();
    build_voxel_mesh(vox, NULL, colors, &grid_mesh, node_depth, depth, offset, scale, 1);
    int octree_count, grid_count;
    zox_tst_vox_face* octree_faces = zox_tst_vox_faces(&octree_mesh, &octree_count);
    zox_tst_vox_face* grid_faces = zox_tst_vox_faces(&grid_mesh, &grid_count);
    zox_tst_check(octree_count > 0, "depth [%i]: the octree built no faces", depth);
    zox_tst_check(octree_count == grid_count, "depth [%i]: faces octree [%i] grid [%i]", depth, octree_count, grid_count);
    if (octree_count == grid_count) {
        int different = 0;
        for (int i = 0; i < octree_count; i++) {
            different += zox_tst_vox_face_compare(&octree_faces[i], &grid_faces[i]) != 0;
        }
        zox_tst_check(!different, "depth [%i]: [%i] of [%i] faces differ (positions or ao colors)",
            depth, different, octree_count);
        // the same walk, so the same order too
        const byte same_order = !memcmp(octree_mesh.vertices->data, grid_mesh.vertices->data,
            octree_mesh.vertices->size * sizeof(float3));
        zox_tst_check(same_order, "depth [%i]: vertices in another order", depth);
    }
    free(octree_faces);
    free(grid_faces);
    zox_tst_vox_mesh_dispose(&octree_mesh);
    zox_tst_vox_mesh_dispose(&grid_mesh);
}

byte zox_tst_vox_mesh_grid_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_vox_mesh_grid_check(ecs* world) {
    (void) world;
    const byte depth = zox_tst_vox_grid_depth;
    const int length = octree_size(depth);
    VoxelNode vox;
    create_VoxelNode(&vox);
    const float radius = length * 0.45f;
    const float center = (length - 1) * 0.5f;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const float dx = x - center, dy = y - center, dz = z - center;
        byte value = dx * dx + dy * dy + dz * dz <= radius * radius ? 1 + (x + y * 3) % 3 : 0;
        if ((x * 7 + y * 13 + z * 5) % 11 == 0) {
            value = 0; // holes
        }
        set_VoxelNode(&vox, depth, (byte3) { x, y, z }, value);
    }
    optimize_reduce_VoxelNode(&vox); // like the game: parents get values, the lower lods
    color_rgb palette[3] = { { 200, 40, 40 }, { 40, 200, 40 }, { 40, 40, 200 } };
    const ColorRGBs colors = { .value = palette, .length = 3 };
    for (int target = depth; target >= 1; target--) {
        zox_tst_vox_mesh_grid_compare(&vox, &colors, depth, target);
    }
    collapse_VoxelNode(&vox);
}

void zox_tsts_chunks3_vox_mesh_grid() {
    zox_tst_add("vox_mesh_grid", zox_tst_vox_mesh_grid_ready, zox_tst_vox_mesh_grid_check, 30);
}
