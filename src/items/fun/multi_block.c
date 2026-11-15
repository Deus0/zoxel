// blocks bigger than one (beds, doors, BlockSize): the base voxel holds the block, the rest of its box holds the
//  realm's shared filler block (solid, draws nothing, BlockFiller), doc/plans/multi_block_vox.md
//  - placing one fills its box, refused unless it's all air (multi_block_place), across chunk edges too
//  - removing any part removes the rest (multi_block_removed): a filler finds its base, the base drops the item
//  - main thread only: they write other chunks' queues

// the chunk holding a world block position, and its local position there, 0 if it isn't loaded
static inline entity terrain_chunk_local(ecs* world, entity terrain, int3 block, byte3* local) {
    if (!zox_valid(terrain) || !zox_has(terrain, ChunkLinks)) {
        return 0;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    const int3 chunk_position = block_position_to_chunk_position(block, depth);
    const entity chunk = int3_hashmap_get(zox_get(terrain, ChunkLinks)->value, chunk_position);
    if (!zox_valid(chunk) || !zox_has(chunk, VoxelNodeQueue)) {
        return 0;
    }
    *local = (byte3) { block.x - chunk_position.x * length, block.y - chunk_position.y * length,
        block.z - chunk_position.z * length };
    return chunk;
}

// a voxel's world block position from its chunk
static inline int3 chunk_block_position(ecs* world, entity chunk, byte3 local) {
    const int length = octree_size(zox_getv(chunk, NodeDepth));
    const int3 c = zox_getv(chunk, ChunkPosition);
    return (int3) { c.x * length + local.x, c.y * length + local.y, c.z * length + local.z };
}

static inline byte terrain_queue_voxel(ecs* world, entity terrain, int3 block, byte value) {
    byte3 local;
    const entity chunk = terrain_chunk_local(world, terrain, block, &local);
    if (!chunk) {
        return 0;
    }
    zox_muter(chunk, VoxelNodeQueue, queue);
    a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = value, .position = local });
    return 1;
}

// the realm's block for a voxel value, 0 for air or none
static inline entity realm_voxel_block(ecs* world, entity realm, int voxel) {
    const BlockLinks* blocks = zox_valid(realm) ? zox_get(realm, BlockLinks) : NULL;
    return blocks && voxel > 0 && voxel <= blocks->length ? blocks->value[voxel - 1] : 0;
}

// the realm's filler voxel value, 0 if it has none
byte realm_filler_voxel(ecs* world, entity realm) {
    const BlockLinks* blocks = zox_valid(realm) ? zox_get(realm, BlockLinks) : NULL;
    for (int i = 0; blocks && i < blocks->length; i++) {
        if (zox_valid(blocks->value[i]) && zox_has(blocks->value[i], BlockFiller)) {
            return i + 1;
        }
    }
    return 0;
}

// a block's size in blocks, 1 x 1 x 1 for most
static inline int3 block_size_of(ecs* world, entity block) {
    return zox_valid(block) && zox_has(block, BlockSize) ? zox_getv(block, BlockSize) : (int3) { 1, 1, 1 };
}

static inline byte block_is_multi(int3 size) {
    return size.x > 1 || size.y > 1 || size.z > 1;
}

// places a block at base, a sized one with its fillers; 0 if any voxel of its box isn't air (or isn't loaded)
byte multi_block_place(ecs* world, entity terrain, entity realm, int3 base, byte value) {
    const int3 size = block_size_of(world, realm_voxel_block(world, realm, value));
    const byte filler = realm_filler_voxel(world, realm);
    if (block_is_multi(size) && !filler) {
        zox_loge("[multi_block_place] the realm has no filler block");
        return 0;
    }
    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++) {
        if (terrain_voxel_at(world, terrain, (int3) { base.x + x, base.y + y, base.z + z }) != 0) {
            return 0;
        }
    }
    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++) {
        terrain_queue_voxel(world, terrain, (int3) { base.x + x, base.y + y, base.z + z }, x || y || z ? filler : value);
    }
    return 1;
}

// a part of a sized block was removed (old its voxel): removes the rest, through its base
void multi_block_removed(ecs* world, entity terrain, entity realm, int3 position, byte old) {
    const entity block = realm_voxel_block(world, realm, old);
    if (!zox_valid(block)) {
        return;
    }
    if (zox_has(block, BlockFiller)) {
        // its base: a sized block below, behind or left of it whose box holds it (sizes are at most 2 for now)
        for (int x = 0; x < 3; x++)
        for (int y = 0; y < 3; y++)
        for (int z = 0; z < 3; z++) {
            if (!x && !y && !z) {
                continue;
            }
            const int3 base = { position.x - x, position.y - y, position.z - z };
            const int voxel = terrain_voxel_at(world, terrain, base);
            const int3 size = block_size_of(world, realm_voxel_block(world, realm, voxel));
            if (block_is_multi(size) && x < size.x && y < size.y && z < size.z) {
                terrain_queue_voxel(world, terrain, base, 0);
                return;
            }
        }
        return;
    }
    const int3 size = block_size_of(world, block);
    if (!block_is_multi(size)) {
        return;
    }
    const byte filler = realm_filler_voxel(world, realm);
    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++) {
        const int3 part = { position.x + x, position.y + y, position.z + z };
        if ((x || y || z) && terrain_voxel_at(world, terrain, part) == filler) {
            terrain_queue_voxel(world, terrain, part, 0);
        }
    }
}
