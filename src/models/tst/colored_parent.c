// tst: colored_parent, a vox model bigger than a chunk (a boss): a colored chunk3 parent (spawn_chunk3_parent_colored)
//  of 2 x 2 x 2 chunks, its palette on it, a ball across all of them
//  - each chunk meshed with its neighbours (chunk_colors_build_system): no faces where its chunks meet, every face on
//    the outside, so the faces of all its chunks are the ball's as one model, counted voxel by voxel here
//  - its chunks have no palette of their own, they draw their parent's
//  - run: bash bsh/tst.sh colored_parent
#define zox_tst_cp_depth 4
entity zox_tst_cpar_parent = 0;
entity zox_tst_cpar_chunks[8];
int zox_tst_cpar_frames = 0;

static inline byte zox_tst_cpar_value(int x, int y, int z) {
    const float c = 15.5f;
    return (x - c) * (x - c) + (y - c) * (y - c) + (z - c) * (z - c) <= 12.5f * 12.5f ? 1 + (x + z) % 2 : 0;
}

byte zox_tst_colored_parent_ready(ecs* world) {
    zox_tst_cpar_frames++;
    if (!zox_tst_cpar_parent) {
        color_rgb colors[2] = { { 180, 60, 60 }, { 60, 60, 180 } };
        const ColorRGBs palette = { .value = colors, .length = 2 };
        zox_tst_cpar_parent = spawn_chunk3_parent_colored(world, prefab_chunk3_parent, &palette, zox_tst_cp_depth, 1.0f);
        const int length = octree_size(zox_tst_cp_depth);
        int n = 0;
        for (int cx = 0; cx < 2; cx++) for (int cy = 0; cy < 2; cy++) for (int cz = 0; cz < 2; cz++) {
            const entity chunk = spawn_chunk3_parent_colored_chunk(world, prefab_vox, zox_tst_cpar_parent, (int3) { cx, cy, cz });
            VoxelNode* voxels = zox_get_mut(chunk, VoxelNode);
            for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
                set_VoxelNode(voxels, zox_tst_cp_depth, (byte3) { x, y, z },
                    zox_tst_cpar_value(cx * length + x, cy * length + y, cz * length + z));
            }
            optimize_reduce_VoxelNode(voxels);
            zox_add(chunk, BuildMesh);
            zox_tst_cpar_chunks[n++] = chunk;
        }
        return 0;
    }
    zox_tst_waiting = "its chunks' meshes";
    for (int i = 0; i < 8; i++) {
        if (zox_has(zox_tst_cpar_chunks[i], BuildMesh) || !zox_get(zox_tst_cpar_chunks[i], MeshVertices)->length) {
            return 0;
        }
    }
    return zox_tst_cpar_frames > 3;
}

void zox_tst_colored_parent_check(ecs* world) {
    int faces = 0;
    for (int i = 0; i < 8; i++) {
        faces += zox_get(zox_tst_cpar_chunks[i], MeshVertices)->length / 4;
        zox_tst_check(!zox_get(zox_tst_cpar_chunks[i], ColorRGBs)->length, "chunk [%i] has its own palette", i);
    }
    int expected = 0;
    for (int x = 0; x < 32; x++) for (int y = 0; y < 32; y++) for (int z = 0; z < 32; z++) {
        if (!zox_tst_cpar_value(x, y, z)) {
            continue;
        }
        for (byte d = 0; d < 6; d++) {
            const int3 n = move_position((int3) { x, y, z }, d);
            const byte inside = n.x >= 0 && n.y >= 0 && n.z >= 0 && n.x < 32 && n.y < 32 && n.z < 32;
            expected += !(inside && zox_tst_cpar_value(n.x, n.y, n.z));
        }
    }
    zox_log("[tst] [metric] colored_parent: [8] chunks, [%i] faces, the ball as one model [%i]", faces, expected);
    zox_tst_check(faces == expected, "its chunks have [%i] faces, the ball as one model [%i] (faces where chunks meet?)",
        faces, expected);
}
