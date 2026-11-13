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
