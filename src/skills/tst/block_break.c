// Exercise the same damage entry point used by melee. Alternate the two faces of a chunk seam.
static entity break_streamer, break_chunk;
static byte3 break_pos;
static int break_trial, break_stage, break_frames;

// Check the -X face of the right chunk in the buffers used by its current draw.
static int right_face_drawn(ecs* world, entity chunk, byte3 pos, float block_scale) {
    entity meshes[zox_chunk_meshes_max];
    const uint count = get_chunk_meshes(world, chunk, meshes);
    for (uint m = 0; m < count; m++) {
        const entity mesh = meshes[m];
        uint indices = zox_getv(mesh, MeshRenderCount);
        if (!indices || zox_has(mesh, Disabled)) continue;
        const byte gpu = zox_has(mesh, GpuMesh);
        if (gpu && !zox_gpu_storage_read(zox_getv(mesh, MeshCommandGPULink).x, 0, sizeof(indices), &indices)) return -1;
        if (!indices) continue;
        const uint stride = gpu ? 4 : 3;
        const size_t bytes = (indices / 6) * 4 * stride * sizeof(float);
        float* vertices = malloc(bytes);
        if (!vertices || !zox_gpu_storage_read(zox_getv(mesh, MeshGPULink).y, 0, bytes, vertices)) {
            free(vertices);
            return -1;
        }
        const float unit = gpu ? 1 : block_scale;
        const float y = (pos.y + 0.5f) * unit, z = (pos.z + 0.5f) * unit;
        for (uint f = 0; f < indices / 6; f++) {
            const float* v = vertices + f * 4 * stride;
            if (v[0] || v[stride] || v[2*stride] || v[3*stride]) continue;
            const float* opposite = v + 2 * stride;
            if (fminf(v[1], opposite[1]) < y && y < fmaxf(v[1], opposite[1]) &&
                fminf(v[2], opposite[2]) < z && z < fmaxf(v[2], opposite[2])) {
                free(vertices);
                return 1;
            }
        }
        free(vertices);
    }
    return 0;
}

static byte block_break_ready(ecs* world) {
    const entity terrain = zox_tst_world_ready(world);
    if (!terrain) return 0;
    if (!break_streamer) {
        break_streamer = spawn_streamer(world, prefab_streamer, terrain, float3_zero);
        zox_setv(break_streamer, StreamerLevel, 1);
        return 0;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    const int side = 1 << depth;
    const entity right = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, (int3){1,-1,0});
    if (!break_stage) {
        if (zox_tst_terrain_busy(world, terrain)) return 0;
        const int x = break_trial % 2;
        break_chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, (int3){x,-1,0});
        if (!zox_valid(right) || !zox_valid(break_chunk)) return 0;
        const BlockLinks* blocks = zox_get(zox_get_link(world, break_chunk, BlockManagerLink), BlockLinks);
        const int column = break_trial / 2;
        if (!x) {
            break_pos = (byte3){x ? 0 : side - 1, 0, column % side};
            int y = side - 1 - column / side;
            for (; y >= 0; y--) {
                break_pos.y = y;
                const byte id = getv_VoxelNode(zox_get(break_chunk, VoxelNode), depth, break_pos);
                const byte other = getv_VoxelNode(zox_get(right, VoxelNode), depth, (byte3){0,y,break_pos.z});
                if (other && other <= blocks->length && zox_has(blocks->value[other-1], BlockHealth) && !zox_has(blocks->value[other-1], BlockInvinsible) && id && id <= blocks->length && !zox_has(blocks->value[id-1], BlockInvinsible) &&
                    zox_has(blocks->value[id-1], BlockHealth)) break;
            }
            zox_tst_check(y >= 0, "no breakable block for trial [%i]", break_trial);
            if (y < 0) return 1;
            if (right_face_drawn(world, right, break_pos, zox_getv(terrain, BlockScale)) != 0) {
                zox_tst_check(0, "starting shared face was exposed or unreadable");
                return 1;
            }
        } else {
            break_pos.x = 0;
        }
        damage_terrain_block(world, break_chunk, break_pos, 0.25f);
        break_stage = 1;
        break_frames = 0;
        return 0;
    }
    const VoxelNode* tree = zox_get(break_chunk, VoxelNode);
    if (break_stage == 1) {
        const VoxelNode* leaf = get_VoxelNode(tree, depth, break_pos);
        const entity health = leaf && is_linked_VoxelNode(leaf) ? get_entity_VoxelNode(leaf) : 0;
        if (!zox_valid(health) || !zox_has(health, StatValue)) {
            if (++break_frames < 30) return 0;
            zox_tst_check(0, "first hit failed to create block health on trial [%i]", break_trial);
            return 1;
        }
        damage_terrain_block(world, break_chunk, break_pos, zox_getv(health, StatValue));
        break_stage = 2;
        break_frames = 0;
        return 0;
    }
    if (getv_VoxelNode(tree, depth, break_pos)) {
        if (++break_frames < 30) return 0;
        zox_tst_check(0, "lethal hit left voxel solid on trial [%i] chunk x [%i] local [%i %i %i]",
            break_trial, break_trial % 2, break_pos.x, break_pos.y, break_pos.z);
        return 1;
    }
    const int expected = !(break_trial % 2);
    const int drawn = right_face_drawn(world, right, break_pos, zox_getv(terrain, BlockScale));
    if (drawn != expected) {
        if (++break_frames < 180) return 0;
        zox_tst_check(0, "voxel is air but displayed face [%i], expected [%i], trial [%i] local [%i %i %i]",
            drawn, expected, break_trial, break_pos.x, break_pos.y, break_pos.z);
        return 1;
    }
    break_stage = 0;
    zox_tst_progress(++break_trial);
    return break_trial == 64;
}

static void block_break_check(ecs* world) {
    (void) world;
    zox_tst_check(break_trial == 64, "removed [%i/64] blocks across the seam", break_trial);
}
