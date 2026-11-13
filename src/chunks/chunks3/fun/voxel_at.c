// the voxel at a world block position in a terrain, -1 if its chunk isn't loaded (its octree locked while read)
//  - footsteps (the block underfoot), tests (the ground under the player)
static inline int terrain_voxel_at(ecs* world, entity terrain, int3 block) {
    if (!zox_valid(terrain) || !zox_has(terrain, ChunkLinks)) {
        return -1;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int3 chunk_position = block_position_to_chunk_position(block, depth);
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const entity chunk = int3_hashmap_get(chunks->value, chunk_position);
    if (!zox_valid(chunk) || !zox_has(chunk, VoxelNode)) {
        return -1;
    }
    const byte3 local = {
        block.x - chunk_position.x * length,
        block.y - chunk_position.y * length,
        block.z - chunk_position.z * length
    };
    spinlock* lock = zox_octree_lock(world, chunk);
    zox_octree_lock_begin(lock);
    const byte voxel = getv_VoxelNode(zox_get(chunk, VoxelNode), depth, local);
    zox_octree_lock_end(lock);
    return voxel;
}

// the solid voxels' bounds in a vox (its max depth voxels, low and high inclusive), 0 if it has none: a model needn't
//  fill its grid (a cookie is flat), its octree locked while read
static inline byte vox_filled_bounds(ecs* world, entity vox, int3* low, int3* high) {
    if (!zox_valid(vox) || !zox_has(vox, VoxelNode) || !zox_has(vox, NodeDepth) || !zox_has(vox, ChunkSize)) {
        return 0;
    }
    const byte depth = zox_getv(vox, NodeDepth);
    // its whole octree, voxels can sit past its ChunkSize
    const int length = octree_size(depth);
    const VoxelNode* voxels = zox_get(vox, VoxelNode);
    byte found = 0;
    spinlock* lock = zox_octree_lock(world, vox);
    zox_octree_lock_begin(lock);
    byte3 p;
    for (p.x = 0; p.x < length; p.x++) {
        for (p.y = 0; p.y < length; p.y++) {
            for (p.z = 0; p.z < length; p.z++) {
                if (!getv_VoxelNode(voxels, depth, p)) {
                    continue;
                }
                if (!found) {
                    *low = *high = byte3_to_int3(p);
                    found = 1;
                }
                low->x = int_min(low->x, p.x);
                low->y = int_min(low->y, p.y);
                low->z = int_min(low->z, p.z);
                high->x = int_max(high->x, p.x);
                high->y = int_max(high->y, p.y);
                high->z = int_max(high->z, p.z);
            }
        }
    }
    zox_octree_lock_end(lock);
    return found;
}
