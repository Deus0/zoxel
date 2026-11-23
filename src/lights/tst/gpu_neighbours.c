// tst: gpu_mesh_neighbours, gpu meshing reads a chunk's neighbours (chunks3/fun/gpu_mesh.c, lights3/fun/smooth_grid.c)
//  - 27 chunks of hills running across their borders, lit air; the middle one meshed lit on the gpu from its smooth
//      grid (a voxel of border from its 26 neighbours), as the game will
//  - faces: exactly a voxel by voxel reference's that looks into the neighbours, a face into a missing chunk only
//      when it's the top (edge_faces 0, as cpu sides)
//  - lights: every corner light against smooth_face_lights over the same 27 chunks
//  - cases: all 27, and three missing (+x, above, -z)
//  - run: bash bsh/tst.sh gpu_mesh_neighbours

#define zox_tst_gpu_neighbour_depth 4

static inline void zox_tst_gpu_neighbour_chunks(VoxelNode* voxels27, LightOctree* lights27, byte depth) {
    const int length = octree_size(depth);
    for (int cx = -1; cx <= 1; cx++) {
        for (int cy = -1; cy <= 1; cy++) {
            for (int cz = -1; cz <= 1; cz++) {
                int i = OCTREE_INDEX(cx, cy, cz);
                octree_create(&voxels27[i], sizeof(VoxelNode), 0);
                octree_create(&lights27[i], sizeof(LightOctree), darklight);
                for (int x = 0; x < length; x++) {
                    for (int y = 0; y < length; y++) {
                        for (int z = 0; z < length; z++) {
                            int gx = cx * length + x;
                            int gy = cy * length + y;
                            int gz = cz * length + z;
                            const int height = length / 2 + (int) (4 * sinf(gx * 0.35f) + 3 * cosf(gz * 0.3f));
                            if (gy < height) {
                                set_VoxelNode(&voxels27[i], depth, (byte3) { x, y, z }, gy < height - 2 ? 2 : 1);
                            } else {
                                set_LightOctree(&lights27[i], depth, (byte3) { x, y, z }, (byte) (60 + ((gx * 7 + gz * 13 + gy * 3) & 127)));
                            }
                        }
                    }
                }
            }
        }
    }
}

static inline void zox_tst_gpu_neighbour_faces(GpuMesher* mesher, const VoxelNode** voxels27, byte depth, const char* name) {
    const int length = octree_size(depth);
    // the reference: a solid voxel's face shows when its neighbour, maybe in the next chunk, is air
    byte* reference = calloc(length * length * length * 6, sizeof(byte));
    int count = 0;
    const VoxelNode* middle = voxels27[OCTREE_INDEX(0, 0, 0)];
    for (int x = 0; x < length; x++) {
        for (int y = 0; y < length; y++) {
            for (int z = 0; z < length; z++) {
                byte p = getv_VoxelNode(middle, depth, (byte3) { x, y, z });
                if (p != 0) {
                    for (int d = 0; d < 6; d++) {
                        int3 n = get_direction_int3(d);
                        n.x += x;
                        n.y += y;
                        n.z += z;
                        byte face = 0;
                        if (n.x >= 0 && n.x < length && n.y >= 0 && n.y < length && n.z >= 0 && n.z < length) {
                            face = !getv_VoxelNode(middle, depth, (byte3) { n.x, n.y, n.z });
                        } else {
                            int sx = n.x < 0 ? -1 : (n.x >= length ? 1 : 0);
                            int sy = n.y < 0 ? -1 : (n.y >= length ? 1 : 0);
                            int sz = n.z < 0 ? -1 : (n.z >= length ? 1 : 0);
                            const int chunk = OCTREE_INDEX(sx, sy, sz);
                            if (!voxels27[chunk]) {
                                face = d == direction_up;
                            } else {
                                int nx = n.x < 0 ? n.x + length : (n.x >= length ? n.x - length : n.x);
                                int ny = n.y < 0 ? n.y + length : (n.y >= length ? n.y - length : n.y);
                                int nz = n.z < 0 ? n.z + length : (n.z >= length ? n.z - length : n.z);
                                face = !getv_VoxelNode(voxels27[chunk], depth, (byte3) { nx, ny, nz });
                            }
                        }
                        if (face) {
                            reference[((x * length + y) * length + z) * 6 + d] = 1;
                            count++;
                        }
                    }
                }
            }
        }
    }
    // the gpu's faces, each once and in the reference
    const uint faces = gpu_mesher_read_faces(mesher);
    float4* vertices = malloc(sizeof(float4) * 4 * (faces ? faces : 1));
    if (faces) {
        zox_gpu_storage_read(mesher->vertices, 0, sizeof(float4) * 4 * faces, vertices);
    }
    byte* seen = calloc(length * length * length * 6, 1);
    int wrong = 0, doubled = 0;
    for (uint f = 0; f < faces; f++) {
        const float4* v = &vertices[f * 4];
        const byte d = gpu_mesh_vertex_direction((int) v[0].w);
        // the voxel: the corners' smallest point, less one on its axis for a positive face
        float3 low = { v[0].x, v[0].y, v[0].z };
        for (byte c = 1; c < 4; c++) {
            low.x = fminf(low.x, v[c].x);
            low.y = fminf(low.y, v[c].y);
            low.z = fminf(low.z, v[c].z);
        }
        int3 p = { (int) low.x, (int) low.y, (int) low.z };
        if (d & 1) {
            if ((d >> 1) == 0) p.x--; else if ((d >> 1) == 1) p.y--; else p.z--;
        }
        if (d >= 6 || p.x < 0 || p.y < 0 || p.z < 0 || p.x >= length || p.y >= length || p.z >= length) {
            wrong++;
            continue;
        }
        const int i = ((p.x * length + p.y) * length + p.z) * 6 + d;
        if (!reference[i]) {
            if (!wrong) {
                zox_tst_check(0, "[%s] gpu face at [%ix%ix%i] direction [%i] isn't in the reference", name, p.x, p.y, p.z, d);
            }
            wrong++;
        } else if (seen[i]) {
            doubled++;
        }
        seen[i] = 1;
    }
    zox_tst_check(faces == (uint) count, "[%s] depth [%i] gpu made [%u] faces, the reference has [%i]", name, depth, faces, count);
    zox_tst_check(!wrong && !doubled, "[%s] depth [%i] faces wrong [%i] doubled [%i]", name, depth, wrong, doubled);
    zox_log("[tst] [%s] depth [%i]: [%u] gpu faces, [%i] reference", name, depth, faces, count);
    free(vertices);
    free(seen);
    free(reference);
}

byte zox_tst_gpu_neighbours_ready(ecs* world) {
    return zox_tst_gpu_mesh_lights_ready(world);
}

void zox_tst_gpu_neighbours_check(ecs* world) {
    (void) world;
    const byte depth = zox_tst_gpu_neighbour_depth;
    const int length = octree_size(depth);
    GpuMesher mesher;
    zox_tst_check(gpu_mesher_create(&mesher, length * length * length * 3), "the gpu mesher couldn't be made");
    if (!mesher.program) {
        return;
    }
    mesher.edge_faces = 0;
    byte solidity[zox_light_solidity_length];
    memset(solidity, 0, sizeof(solidity));
    solidity[0] = solidity[1] = 1;
    VoxelNode voxels[27];
    LightOctree lights[27];
    zox_tst_gpu_neighbour_chunks(voxels, lights, depth);
    const VoxelNode* voxels27[27];
    const LightOctree* lights27[27];
    for (int i = 0; i < 27; i++) {
        voxels27[i] = &voxels[i];
        lights27[i] = &lights[i];
    }
    // all 27 there
    zox_tst_gpu_light_compare(&mesher, voxels27, lights27, solidity, depth, "neighbours");
    zox_tst_gpu_neighbour_faces(&mesher, voxels27, depth, "neighbours");
    // three missing: faces into them culled but the top, their lights past the world
    voxels27[OCTREE_INDEX(1, 0, 0)] = NULL;
    lights27[OCTREE_INDEX(1, 0, 0)] = NULL;
    voxels27[OCTREE_INDEX(0, 1, 0)] = NULL;
    lights27[OCTREE_INDEX(0, 1, 0)] = NULL;
    voxels27[OCTREE_INDEX(0, 0, -1)] = NULL;
    lights27[OCTREE_INDEX(0, 0, -1)] = NULL;
    zox_tst_gpu_light_compare(&mesher, voxels27, lights27, solidity, depth, "missing");
    zox_tst_gpu_neighbour_faces(&mesher, voxels27, depth, "missing");
    for (int i = 0; i < 27; i++) {
        close_VoxelNode(&voxels[i]);
        close_LightOctree(&lights[i]);
    }
    gpu_mesher_dispose(&mesher);
}

void zox_tsts_lights_gpu_neighbours(ecs* world) {
    (void) world;
    zox_tst_add("gpu_mesh_neighbours", zox_tst_gpu_neighbours_ready, zox_tst_gpu_neighbours_check, 60);
}
