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
        if (zox_has(zox_tst_cpar_chunks[i], BuildMesh) || !mesh_index_count(world, zox_tst_cpar_chunks[i]) ||
            vox_gpu_pending(world, zox_tst_cpar_chunks[i])) {
            return 0;
        }
    }
    return zox_tst_cpar_frames > 3;
}

void zox_tst_colored_parent_check(ecs* world) {
    int faces = 0;
    for (int i = 0; i < 8; i++) {
        faces += mesh3_voxel_faces(world, zox_tst_cpar_chunks[i], 1.0f);  // as voxel faces: however they merge
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
    zox_log("[tst] [metric] colored_parent: [8] chunks, [%i] faces, the ball as one model [%i], meshed on the [%s]", faces, expected,
        zox_has(zox_tst_cpar_chunks[0], GpuMesh) ? "gpu" : "cpu");
    zox_tst_check(faces == expected, "its chunks have [%i] faces, the ball as one model [%i] (faces where chunks meet?)",
        faces, expected);
}

// tst: colored_parent_shell, the same 2 x 2 x 2 model, a solid core in a see through shell across its chunks (a big
//  slime): each chunk's solid mesh and its see through child (set_chunk3_parent_colored_materials), meshed with their
//  neighbours, their faces the model's as one, both kinds counted voxel by voxel; each see through child placed as
//  its chunk (it follows its transform)
//  - run: bash bsh/tst.sh colored_parent_shell
entity zox_tst_cpar_material = 0;

static inline byte zox_tst_cpar_shell_value(int x, int y, int z) {
    const float c = 15.5f;
    const float d2 = (x - c) * (x - c) + (y - c) * (y - c) + (z - c) * (z - c);
    return d2 <= 11.0f * 11.0f ? 1 : (d2 <= 12.5f * 12.5f ? 2 : 0);
}

// a chunk's see through child, 0 if none
static inline entity zox_tst_cpar_shell_of(ecs* world, entity chunk) {
    entity found = 0;
    iter it = zox_children(world, chunk);
    while (zox_children_next(it)) {
        for (int i = 0; i < it.count; i++) {
            found = zox_has(it.entities[i], TransparentMesh) ? it.entities[i] : found;
        }
    }
    return found;
}

byte zox_tst_colored_parent_shell_ready(ecs* world) {
    zox_tst_cpar_frames++;
    if (!zox_tst_cpar_parent) {
        color_rgb colors[2] = { { 200, 60, 60 }, { 90, 220, 110 } };
        const ColorRGBs palette = { .value = colors, .length = 2 };
        zox_tst_cpar_parent = spawn_chunk3_parent_colored(world, prefab_chunk3_parent, &palette, zox_tst_cp_depth, 1.0f);
        const int length = octree_size(zox_tst_cp_depth);
        int n = 0;
        for (int cx = 0; cx < 2; cx++) for (int cy = 0; cy < 2; cy++) for (int cz = 0; cz < 2; cz++) {
            const entity chunk = spawn_chunk3_parent_colored_chunk(world, prefab_vox, zox_tst_cpar_parent, (int3) { cx, cy, cz });
            VoxelNode* voxels = zox_get_mut(chunk, VoxelNode);
            for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
                set_VoxelNode(voxels, zox_tst_cp_depth, (byte3) { x, y, z },
                    zox_tst_cpar_shell_value(cx * length + x, cy * length + y, cz * length + z));
            }
            optimize_reduce_VoxelNode(voxels);
            zox_tst_cpar_chunks[n++] = chunk;
        }
        zox_tst_cpar_material = spawn_vox_material_see_through(world, 0.5f);
        const byte ids[2] = { zox_block_material_solid, zox_block_material_liquid };
        set_chunk3_parent_colored_materials(world, zox_tst_cpar_parent, ids, 2, zox_tst_cpar_material);
        return 0;
    }
    zox_tst_waiting = "its chunks' solid and see through meshes";
    for (int i = 0; i < 8; i++) {
        const entity shell = zox_tst_cpar_shell_of(world, zox_tst_cpar_chunks[i]);
        if (zox_has(zox_tst_cpar_chunks[i], BuildMesh) || !shell || zox_has(shell, MeshDirty) ||
            vox_gpu_pending(world, zox_tst_cpar_chunks[i]) || vox_gpu_pending(world, shell)) {
            return 0;
        }
    }
    return zox_tst_cpar_frames > 5;
}

void zox_tst_colored_parent_shell_check(ecs* world) {
    int solid = 0, shell = 0, misplaced = 0;
    for (int i = 0; i < 8; i++) {
        const entity chunk = zox_tst_cpar_chunks[i];
        const entity child = zox_tst_cpar_shell_of(world, chunk);
        solid += mesh3_voxel_faces(world, chunk, 1.0f);
        shell += mesh3_voxel_faces(world, child, 1.0f);
        const float4x4 a = zox_getv(chunk, TransformMatrix), b = zox_getv(child, TransformMatrix);
        misplaced += memcmp(&a, &b, sizeof(float4x4)) != 0;
    }
    int solid_expected = 0, shell_expected = 0;
    for (int x = 0; x < 32; x++) for (int y = 0; y < 32; y++) for (int z = 0; z < 32; z++) {
        const byte v = zox_tst_cpar_shell_value(x, y, z);
        if (!v) {
            continue;
        }
        for (byte d = 0; d < 6; d++) {
            const int3 n = move_position((int3) { x, y, z }, d);
            const byte inside = n.x >= 0 && n.y >= 0 && n.z >= 0 && n.x < 32 && n.y < 32 && n.z < 32;
            const byte other = inside ? zox_tst_cpar_shell_value(n.x, n.y, n.z) : 0;
            // a solid hides any face, a see through voxel only its own kind's (block_material_hides)
            const byte hidden = other == 1 || (other && other == v);
            if (!hidden) {
                solid_expected += v == 1;
                shell_expected += v == 2;
            }
        }
    }
    zox_log("[tst] [metric] colored_parent_shell: solid [%i] faces (as one model [%i]), see through [%i] ([%i]), see "
        "through children not placed as their chunk [%i]", solid, solid_expected, shell, shell_expected, misplaced);
    zox_tst_check(solid == solid_expected, "its chunks' solid meshes have [%i] faces, the model as one [%i]", solid,
        solid_expected);
    zox_tst_check(shell == shell_expected, "its chunks' see through meshes have [%i] faces, the model as one [%i]", shell,
        shell_expected);
    zox_tst_check(!misplaced, "[%i] see through children aren't placed as their chunk", misplaced);
}
