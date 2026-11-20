// tst: octree_flatten and gpu_mesh, gpu meshing (fun/flatten.c, fun/gpu_mesh.c, doc/plans/gpu_meshing.md)
//  - octree_flatten: every voxel of a flattened octree reads back as getv_VoxelNode reads the octree, at its depth
//      and at lower ones (lods), cpu only
//  - gpu_mesh: the compute shader's mesh has exactly the faces a voxel by voxel reference has (a solid voxel's face
//      next to air or the chunk's edge), each once, with the right corners, at its depth and lower ones
//  - gpu_mesh logs [tst] [metric] lines: gpu mesh time vs the cpu's sides (chunk_sides_system's part of meshing)
//  - run: bash bsh/tst.sh octree_flatten, bash bsh/tst.sh gpu_mesh

#define zox_tst_gpu_cases 7

static const char* zox_tst_gpu_case_names[zox_tst_gpu_cases] = {
    "empty", "one voxel", "solid", "sphere", "noise", "checkers", "terrain" };

static inline uint zox_tst_gpu_rand(uint* state) {
    uint x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

// fills a chunk of length^3 voxels with case c, optimized like generated chunks (voxel_octree_optimize_system):
//  uniform areas are big nodes, and parents hold a value (the most common solid one), so lods have voxels
static inline void zox_tst_gpu_fill(VoxelNode* node, byte depth, int c) {
    create_VoxelNode(node);
    const int length = octree_size(depth);
    const float half = length / 2.0f;
    uint state = 666 + c;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        byte v = 0;
        switch (c) {
            case 1: v = x == length / 2 && y == 1 && z == length - 1 ? 3 : 0; break;
            case 2: v = 1; break;
            case 3: {
                const float dx = x + 0.5f - half, dy = y + 0.5f - half, dz = z + 0.5f - half;
                v = dx * dx + dy * dy + dz * dz <= half * half * 0.8f ? 1 + (y * 3 / length) : 0;
                break;
            }
            case 4: v = zox_tst_gpu_rand(&state) % 3 == 0 ? 1 + zox_tst_gpu_rand(&state) % 4 : 0; break;
            case 5: v = (x + y + z) % 2 ? 2 : 0; break;
            case 6: {
                // hills: a height from two waves, stone under dirt under grass
                const int height = (int) (length * (0.4f + 0.15f * sinf(x * 0.4f) + 0.1f * cosf(z * 0.3f)));
                v = y < height - 3 ? 3 : (y < height - 1 ? 2 : (y < height ? 1 : 0));
                break;
            }
        }
        if (v) {
            set_VoxelNode(node, depth, (byte3) { x, y, z }, v);
        }
    }
    optimize_reduce_VoxelNode(node);
}

// # octree_flatten

byte zox_tst_octree_flatten_ready(ecs* world) {
    (void) world;
    return 1;
}

void zox_tst_octree_flatten_check(ecs* world) {
    (void) world;
    const byte depth = 5;
    for (int c = 0; c < zox_tst_gpu_cases; c++) {
        VoxelNode voxels;
        zox_tst_gpu_fill(&voxels, depth, c);
        uint* nodes = NULL;
        const uint count = flatten_VoxelNode(&voxels, &nodes);
        zox_tst_check(count && nodes, "[%s] didn't flatten", zox_tst_gpu_case_names[c]);
        if (!count) {
            collapse_VoxelNode(&voxels);
            continue;
        }
        // its depth and two lods
        int wrong = 0;
        for (int d = depth; d >= depth - 2; d--) {
            const int length = octree_size(d);
            for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
                const byte expected = getv_VoxelNode(&voxels, d, (byte3) { x, y, z });
                const byte flat = flat_voxel_get(nodes, d, (int3) { x, y, z });
                if (flat != expected) {
                    if (!wrong) {
                        zox_tst_check(0, "[%s] voxel [%ix%ix%i] at depth [%i] flat [%i] octree [%i]",
                            zox_tst_gpu_case_names[c], x, y, z, d, flat, expected);
                    }
                    wrong++;
                }
            }
        }
        zox_tst_check(!wrong, "[%s] [%i] voxels read back wrong", zox_tst_gpu_case_names[c], wrong);
        zox_log("[tst] [%s] flattened to [%u] nodes ([%u] bytes)", zox_tst_gpu_case_names[c], count, count * 4);
        free(nodes);
        collapse_VoxelNode(&voxels);
    }
}

// # gpu_mesh

// the faces a voxel by voxel reference has: face (x, y, z, direction) is set when the voxel is solid and its
//  neighbour that way is air or outside the chunk, returns how many
static inline int zox_tst_gpu_reference(const VoxelNode* voxels, byte depth, byte* faces) {
    const int length = octree_size(depth);
    int count = 0;
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const int i = (x * length + y) * length + z;
        if (!getv_VoxelNode(voxels, depth, (byte3) { x, y, z })) {
            continue;
        }
        for (byte d = 0; d < 6; d++) {
            const int3 n = int3_add((int3) { x, y, z }, get_direction_int3(d));
            const byte air = n.x < 0 || n.y < 0 || n.z < 0 || n.x >= length || n.y >= length || n.z >= length ||
                !getv_VoxelNode(voxels, depth, int3_to_byte3(n));
            faces[i * 6 + d] = air;
            count += air;
        }
    }
    return count;
}

// the gpu mesh against the reference: each face once, matching it, its 4 corners the voxel's face
static inline void zox_tst_gpu_compare(GpuMesher* mesher, const VoxelNode* voxels, byte depth, const char* name) {
    const int length = octree_size(depth);
    const int cells = length * length * length;
    byte* expected = calloc(cells * 6, 1);
    byte* seen = calloc(cells * 6, 1);
    const int reference = zox_tst_gpu_reference(voxels, depth, expected);
    gpu_mesh_VoxelNode(mesher, voxels, depth);
    const uint faces = gpu_mesher_read_faces(mesher);
    zox_tst_check(faces == (uint) reference, "[%s] depth [%i] gpu made [%u] faces, the reference has [%i]",
        name, depth, faces, reference);
    float4* vertices = malloc(sizeof(float4) * 4 * (faces ? faces : 1));
    uint* indices = malloc(sizeof(uint) * 6 * (faces ? faces : 1));
    int wrong = 0, doubled = 0, bad_shape = 0;
    if (faces) {
        zox_gpu_storage_read(mesher->vertices, 0, sizeof(float4) * 4 * faces, vertices);
        zox_gpu_storage_read(mesher->indices, 0, sizeof(uint) * 6 * faces, indices);
    }
    for (uint f = 0; f < faces; f++) {
        const float4* v = &vertices[f * 4];
        const int w = (int) v[0].w;
        const byte d = gpu_mesh_vertex_direction(w);
        const byte voxel = gpu_mesh_vertex_voxel(w);
        // the voxel: the corners' smallest point, less the face's offset on its axis when it's a positive face
        float3 low = { v[0].x, v[0].y, v[0].z };
        for (byte c = 1; c < 4; c++) {
            low.x = fminf(low.x, v[c].x);
            low.y = fminf(low.y, v[c].y);
            low.z = fminf(low.z, v[c].z);
        }
        int3 p = { (int) low.x, (int) low.y, (int) low.z };
        const byte axis = d >> 1;
        if (d & 1) {
            if (axis == 0) p.x--; else if (axis == 1) p.y--; else p.z--;
        }
        if (d >= 6 || p.x < 0 || p.y < 0 || p.z < 0 || p.x >= length || p.y >= length || p.z >= length) {
            bad_shape++;
            continue;
        }
        // the face is a unit square on its plane: every corner has the same coordinate on its axis, all 4 differ
        byte flat = 1;
        for (byte c = 1; c < 4; c++) {
            const float a0 = axis == 0 ? v[0].x : (axis == 1 ? v[0].y : v[0].z);
            const float ac = axis == 0 ? v[c].x : (axis == 1 ? v[c].y : v[c].z);
            flat &= a0 == ac;
            // the same voxel and direction (the light and corner side differ per corner)
            flat &= gpu_mesh_vertex_voxel((int) v[c].w) == voxel && gpu_mesh_vertex_direction((int) v[c].w) == d;
        }
        const uint* idx = &indices[f * 6];
        flat &= idx[0] == f * 4 && idx[1] == f * 4 + 1 && idx[2] == f * 4 + 2 && idx[3] == f * 4 && idx[4] == f * 4 + 2 && idx[5] == f * 4 + 3;
        bad_shape += !flat;
        const int i = ((p.x * length + p.y) * length + p.z) * 6 + d;
        if (!expected[i] || voxel != getv_VoxelNode(voxels, depth, int3_to_byte3(p))) {
            if (!wrong) {
                zox_tst_check(0, "[%s] depth [%i] gpu face at [%ix%ix%i] direction [%i] voxel [%i] isn't in the reference",
                    name, depth, p.x, p.y, p.z, d, voxel);
            }
            wrong++;
        } else if (seen[i]) {
            doubled++;
        }
        seen[i] = 1;
    }
    zox_tst_check(!wrong && !doubled && !bad_shape, "[%s] depth [%i] faces wrong [%i] doubled [%i] bad shape or indices [%i]",
        name, depth, wrong, doubled, bad_shape);
    zox_log("[tst] [%s] depth [%i]: [%u] gpu faces, [%i] reference", name, depth, faces, reference);
    free(vertices);
    free(indices);
    free(expected);
    free(seen);
}

// times the gpu (the grid's fill, upload, mesh, wait) and the cpu's sides (chunk_sides_system's part of meshing) for one chunk
static inline void zox_tst_gpu_time(GpuMesher* mesher, const VoxelNode* voxels, byte depth, const char* name) {
    const int runs = 20;
    double start = get_time_ms();
    for (int r = 0; r < runs; r++) {
        gpu_mesh_VoxelNode(mesher, voxels, depth);
        zox_gpu_finish();
    }
    const double gpu_ms = (get_time_ms() - start) / runs;
    const int length = octree_size(depth) + 2;
    byte* grid = calloc((size_t) length * length * length, 1);
    octree_fill_grid(voxels, sizeof(VoxelNode), octree_type_offset_VoxelNode, 0, depth, int3_zero, (int3) { 1, 1, 1 },
        grid, length, 0);
    start = get_time_ms();
    for (int r = 0; r < runs; r++) {
        gpu_mesh_grid(mesher, grid, NULL, depth, 1u << OCTREE_INDEX(0, 0, 0), 0, NULL);
        zox_gpu_finish();
    }
    const double gpu_only_ms = (get_time_ms() - start) / runs;
    free(grid);
    const VoxelNode* neighbors[6] = { NULL };
    const byte neighbor_depths[6] = { depth, depth, depth, depth, depth, depth };
    init_side_child_indices();
    start = get_time_ms();
    for (int r = 0; r < runs; r++) {
        SidesOctree sides;
        create_SidesOctree(&sides);
        build_sides_dig(NULL, zox_block_material_solid, voxels, neighbors, neighbor_depths, voxels, &sides, depth, 0, byte3_zero, 0);
        collapse_SidesOctree(&sides);
    }
    const double cpu_sides_ms = (get_time_ms() - start) / runs;
    zox_log("[tst] [metric] gpu_mesh [%s] depth [%i]: gpu [%.3fms] (without the grid's fill [%.3fms]), cpu sides only [%.3fms]",
        name, depth, gpu_ms, gpu_only_ms, cpu_sides_ms);
}

byte zox_tst_gpu_mesh_ready(ecs* world) {
    (void) world;
    zox_tst_waiting = "a compute capable gl context (OpenGL ES 3.1+ or OpenGL 4.3+)";
    return zox_gpu_compute_supported();
}

void zox_tst_gpu_mesh_check(ecs* world) {
    (void) world;
    const byte depth = 5;
    const int length = octree_size(depth);
    GpuMesher mesher;
    // the most faces a chunk has: checkers, every solid voxel shows all 6
    const uint max_faces = length * length * length * 3;
    zox_tst_check(gpu_mesher_create(&mesher, max_faces), "the gpu mesher couldn't be made (compute shader compile or link)");
    if (!mesher.program) {
        return;
    }
    for (int c = 0; c < zox_tst_gpu_cases; c++) {
        VoxelNode voxels;
        zox_tst_gpu_fill(&voxels, depth, c);
        for (int d = depth; d >= depth - 2; d--) {
            zox_tst_gpu_compare(&mesher, &voxels, d, zox_tst_gpu_case_names[c]);
        }
        if (c == 4 || c == 6) {
            zox_tst_gpu_time(&mesher, &voxels, depth, zox_tst_gpu_case_names[c]);
        }
        collapse_VoxelNode(&voxels);
    }
    // a lower block (flowing water, its height 3 eighths): one alone, its top and its sides' tops at 3 / 8; one under
    //  it (no top showing) full
    {
        byte heights[255] = { 0 };
        heights[1] = 3;     // voxel 2
        gpu_mesher_set_heights(&mesher, heights);
        VoxelNode voxels = { 0 };
        const byte small = 2;
        set_VoxelNode(&voxels, small, (byte3) { 1, 0, 1 }, 2);
        set_VoxelNode(&voxels, small, (byte3) { 1, 1, 1 }, 2);
        set_VoxelNode(&voxels, small, (byte3) { 2, 0, 2 }, 2);
        gpu_mesh_VoxelNode(&mesher, &voxels, small);
        const uint faces = gpu_mesher_read_faces(&mesher);
        float4 vertices[4 * 32];
        zox_gpu_storage_read(mesher.vertices, 0, sizeof(float4) * 4 * (faces < 32 ? faces : 32), vertices);
        int lowered = 0, wrong = 0;
        for (uint i = 0; i < 4 * faces && i < 4 * 32; i++) {
            const float y = vertices[i].y;
            const float fraction = y - floorf(y);
            if (fabsf(fraction - 3 / 8.0f) < 0.001f) {
                lowered++;
            } else if (fraction != 0) {
                wrong++;
            }
        }
        // the alone block (2,0,2): its top's 4 and its 4 sides' tops (2 each): 12 corners at 3/8; the column's top
        //  block (1,1,1): the same at 1 + 3/8, 12 more
        zox_log("[tst] [metric] gpu_mesh heights: [%i] faces, [%i] corners at 3/8 of a block, [%i] at other heights", faces, lowered, wrong);
        zox_tst_check(lowered == 24 && !wrong, "a lower block's corners: [%i] at its height (not 24), [%i] wrong", lowered, wrong);
        memset(heights, 0, sizeof(heights));
        gpu_mesher_set_heights(&mesher, heights);
        collapse_VoxelNode(&voxels);
    }
    gpu_mesher_dispose(&mesher);
}

// # gpu_mesh_grow

// a mesher sized by a guess too small: the mesh draws only what it wrote, its count comes back frames later without
//  waiting (a fence), the mesher grows to it and the mesh again has every face
#define zox_tst_gpu_grow_guess 16
GpuMesher zox_tst_gpu_grow_mesher;
VoxelNode zox_tst_gpu_grow_voxels;
byte zox_tst_gpu_grow_stage = 0;
int zox_tst_gpu_grow_frames = 0;
uint zox_tst_gpu_grow_wanted = 0;

byte zox_tst_gpu_grow_ready(ecs* world) {
    (void) world;
    zox_tst_progress(zox_tst_gpu_grow_stage * 1000 + zox_tst_gpu_grow_frames);
    if (zox_tst_gpu_grow_stage == 0) {
        zox_tst_waiting = "a compute capable gl context (OpenGL ES 3.1+ or OpenGL 4.3+)";
        if (!zox_gpu_compute_supported()) {
            return 0;
        }
        zox_tst_check(gpu_mesher_create(&zox_tst_gpu_grow_mesher, zox_tst_gpu_grow_guess), "the gpu mesher couldn't be made");
        if (!zox_tst_gpu_grow_mesher.program) {
            return 1;
        }
        zox_tst_gpu_fill(&zox_tst_gpu_grow_voxels, 5, 6);
        gpu_mesh_VoxelNode(&zox_tst_gpu_grow_mesher, &zox_tst_gpu_grow_voxels, 5);
        zox_tst_gpu_grow_stage = 1;
        return 0;
    }
    zox_tst_waiting = "the mesh's face count back from the gpu";
    zox_tst_gpu_grow_frames++;
    return gpu_mesher_counted(&zox_tst_gpu_grow_mesher, &zox_tst_gpu_grow_wanted);
}

void zox_tst_gpu_grow_check(ecs* world) {
    (void) world;
    GpuMesher* mesher = &zox_tst_gpu_grow_mesher;
    if (!mesher->program) {
        return;
    }
    const byte depth = 5;
    const int length = octree_size(depth);
    byte* expected = calloc(length * length * length * 6, 1);
    const int reference = zox_tst_gpu_reference(&zox_tst_gpu_grow_voxels, depth, expected);
    free(expected);
    zox_tst_check(zox_tst_gpu_grow_wanted == (uint) reference, "the count [%u] isn't the reference's [%i] faces",
        zox_tst_gpu_grow_wanted, reference);
    // only what fit was written, and only that is drawn
    uint indices = 0;
    zox_gpu_storage_read(mesher->command, 0, sizeof(uint), &indices);
    zox_tst_check(indices == zox_tst_gpu_grow_guess * 6, "the draw has [%u] indices, [%i] faces fit",
        indices, zox_tst_gpu_grow_guess);
    uint again = 0;
    zox_tst_check(!gpu_mesher_counted(mesher, &again), "a mesh's count read twice");
    zox_tst_check(gpu_mesher_grow(mesher, zox_tst_gpu_grow_wanted) && mesher->max_faces >= zox_tst_gpu_grow_wanted,
        "the mesher didn't grow to [%u] faces (room for [%u])", zox_tst_gpu_grow_wanted, mesher->max_faces);
    zox_tst_check(!gpu_mesher_grow(mesher, zox_tst_gpu_grow_wanted), "the mesher grew when the faces fit");
    // meshed again, every face (zox_tst_gpu_compare meshes and waits)
    zox_tst_gpu_compare(mesher, &zox_tst_gpu_grow_voxels, depth, "grown");
    zox_log("[tst] [metric] gpu_mesh_grow: [%u] faces counted [%i] frames after the mesh, room for [%i] grew to [%u]",
        zox_tst_gpu_grow_wanted, zox_tst_gpu_grow_frames, zox_tst_gpu_grow_guess, mesher->max_faces);
    collapse_VoxelNode(&zox_tst_gpu_grow_voxels);
    gpu_mesher_dispose(mesher);
}

void zox_tsts_chunks3_gpu_mesh() {
    zox_tst_add("octree_flatten", zox_tst_octree_flatten_ready, zox_tst_octree_flatten_check, 30);
    zox_tst_add("gpu_mesh", zox_tst_gpu_mesh_ready, zox_tst_gpu_mesh_check, 60);
    zox_tst_add("gpu_mesh_grow", zox_tst_gpu_grow_ready, zox_tst_gpu_grow_check, 60);
}
