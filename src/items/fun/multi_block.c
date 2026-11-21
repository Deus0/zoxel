// blocks bigger than one (beds, doors, BlockSize): the base voxel holds the block, the rest of its box holds the
//  realm's shared filler block (solid, draws nothing, BlockFiller), doc/plans/multi_block_vox.md
//  - placing one turns it to the placer's facing: its box turned about its base, its base's block data its direction,
//    each filler's its offset back to its base (chunks3/fun/block_data.c); refused unless its box is all air and
//    solid ground is under each of its bottom cells
//  - removing any part removes the rest (multi_block_removed): a filler its base, a base its fillers, the base drops it;
//    so does removing the ground under one, or under grass and flowers (block_needs_ground)
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

static inline byte terrain_queue_voxel(ecs* world, entity terrain, int3 block, byte value, byte data) {
    byte3 local;
    const entity chunk = terrain_chunk_local(world, terrain, block, &local);
    if (!chunk) {
        return 0;
    }
    zox_muter(chunk, VoxelNodeQueue, queue);
    a_VoxelNodeQueue(queue, (VoxelNodeUpdate) { .value = value, .data = data, .position = local });
    return 1;
}

// a voxel's block data, 0 if none or its chunk isn't loaded
static inline byte terrain_block_data_at(ecs* world, entity terrain, int3 block) {
    byte3 local;
    const entity chunk = terrain_chunk_local(world, terrain, block, &local);
    if (!chunk || !zox_has(chunk, BlockDatas)) {
        return 0;
    }
    spinlock* lock = zox_octree_lock(world, chunk);
    zox_octree_lock_begin(lock);
    const byte data = get_block_data(zox_get(chunk, BlockDatas), local);
    zox_octree_lock_end(lock);
    return data;
}

// the realm's block for a voxel value, 0 for air or none
static inline entity realm_voxel_block(ecs* world, entity realm, int voxel) {
    const BlockLinks* blocks = zox_valid(realm) ? zox_get(realm, BlockLinks) : NULL;
    return blocks && voxel > 0 && voxel <= blocks->length ? blocks->value[voxel - 1] : 0;
}

// a block's size in blocks, 1 x 1 x 1 for most
static inline int3 block_size_of(ecs* world, entity block) {
    return zox_valid(block) && zox_has(block, BlockSize) ? zox_getv(block, BlockSize) : (int3) { 1, 1, 1 };
}

static inline byte block_is_multi(int3 size) {
    return size.x > 1 || size.y > 1 || size.z > 1;
}

// a sized block's base from any of its parts (itself, or a filler back by its offset): its block kind, 0 if none
static inline entity multi_block_base_at(ecs* world, entity terrain, entity realm, int3 block, int3* base) {
    entity b = realm_voxel_block(world, realm, terrain_voxel_at(world, terrain, block));
    *base = block;
    if (zox_valid(b) && zox_has(b, BlockFiller)) {
        const byte data = terrain_block_data_at(world, terrain, block);
        if (!data) {
            return 0;
        }
        *base = int3_add(block, block_offset_decode(data));
        b = realm_voxel_block(world, realm, terrain_voxel_at(world, terrain, *base));
    }
    return zox_valid(b) ? b : 0;
}

// a solid block (its collider) at a world block position
static inline byte block_is_solid_at(ecs* world, entity terrain, entity realm, int3 block) {
    const entity b = realm_voxel_block(world, realm, terrain_voxel_at(world, terrain, block));
    return zox_valid(b) && (!zox_has(b, BlockCollider) || zox_getv(b, BlockCollider) != zox_block_air);
}

// places a block at base, a sized one turned to direction with its fillers; 0 if any voxel of its box isn't air
byte multi_block_place(ecs* world, entity terrain, entity realm, int3 base, byte value, byte direction) {
    const int3 size = block_size_of(world, realm_voxel_block(world, realm, value));
    if (!block_is_multi(size)) {
        if (terrain_voxel_at(world, terrain, base) != 0) {
            return 0;
        }
        return terrain_queue_voxel(world, terrain, base, value, 0);
    }
    const byte filler = block_filler_voxel(world, realm, realm_voxel_block(world, realm, value));
    if (!filler) {
        zox_loge("[multi_block_place] the realm has no filler block");
        return 0;
    }
    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++) {
        const int3 o = block_direction_offset(direction, (int3) { x, y, z });
        if (terrain_voxel_at(world, terrain, int3_add(base, o)) != 0) {
            return 0;
        }
        // stood on ground: a solid block under each of its bottom cells (half a bed hung over air)
        if (!y && !block_is_solid_at(world, terrain, realm, int3_add(base, (int3) { o.x, o.y - 1, o.z }))) {
            return 0;
        }
    }
    for (int x = 0; x < size.x; x++)
    for (int y = 0; y < size.y; y++)
    for (int z = 0; z < size.z; z++) {
        const int3 o = block_direction_offset(direction, (int3) { x, y, z });
        if (!x && !y && !z) {
            terrain_queue_voxel(world, terrain, base, value, 1 + direction);
        } else {
            terrain_queue_voxel(world, terrain, int3_add(base, o), filler, block_offset_encode(int3_scale(o, -1)));
        }
    }
    return 1;
}

// a block that stands on the one under it: a bed's or door's part, or a model you walk through (grass, flowers,
//  debris), it goes when that does
static inline byte block_needs_ground(ecs* world, entity block) {
    return zox_valid(block) && (zox_has(block, BlockFiller) || block_is_multi(block_size_of(world, block)) ||
        (zox_has(block, BlockVox) && zox_has(block, BlockCollider) && zox_getv(block, BlockCollider) == zox_block_air));
}

// a part of a sized block was removed (old its voxel, old_data its block data): removes the rest
//  - and what stood on it (block_needs_ground): grass, flowers, a bed's part
void multi_block_removed(ecs* world, entity terrain, entity realm, int3 position, byte old, byte old_data) {
    const int3 above = { position.x, position.y + 1, position.z };
    if (block_needs_ground(world, realm_voxel_block(world, realm, terrain_voxel_at(world, terrain, above)))) {
        terrain_queue_voxel(world, terrain, above, 0, 0);
    }
    const entity block = realm_voxel_block(world, realm, old);
    if (!zox_valid(block)) {
        return;
    }
    if (zox_has(block, BlockFiller)) {
        // its base, back by its offset
        if (old_data) {
            const int3 base = int3_add(position, block_offset_decode(old_data));
            if (block_is_multi(block_size_of(world, realm_voxel_block(world, realm, terrain_voxel_at(world, terrain, base))))) {
                terrain_queue_voxel(world, terrain, base, 0, 0);
            }
        }
        return;
    }
    if (!block_is_multi(block_size_of(world, block))) {
        return;
    }
    // its fillers: those round it whose offsets lead back here
    const byte filler = block_filler_voxel(world, realm, block);
    const int r = block_offset_range;
    for (int x = -r; x <= r; x++)
    for (int y = -r; y <= r; y++)
    for (int z = -r; z <= r; z++) {
        const int3 part = { position.x + x, position.y + y, position.z + z };
        if (!(x || y || z) || terrain_voxel_at(world, terrain, part) != filler) {
            continue;
        }
        const byte data = terrain_block_data_at(world, terrain, part);
        if (data && int3_equals(int3_add(part, block_offset_decode(data)), position)) {
            terrain_queue_voxel(world, terrain, part, 0, 0);
        }
    }
}
