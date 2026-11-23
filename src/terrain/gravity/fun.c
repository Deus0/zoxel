// a field's value: 0 the realm's own gravity (zox_default_gravity, down), else where it pulls, 1 + axis * 2 + (1 if
//  it pulls along +axis): 1 -x, 2 +x, 3 -y, 4 +y, 5 -z, 6 +z
#define gravity_default 0
#define gravity_up 4            // a gravity cube's flip of the realm's down
#define gravity_radius 4        // blocks round a gravity cube it flips (a sphere, to voxel centres)
#define gravity_sources_max 64  // cubes switched on that can reach one chunk

static inline float3 gravity_pull(byte value) {
    if (value == gravity_default) {
        return zox_default_gravity;
    }
    const float strength = float3_length(zox_default_gravity);
    float3 pull = float3_zero;
    *collision_axis_ptr(&pull, (value - 1) / 2) = (value - 1) % 2 ? strength : -strength;
    return pull;
}

typedef struct {
    int count;
    int3 block[gravity_sources_max];
} gravity_sources;

// the gravity cubes switched on in a chunk and the 26 round it (they reach one chunk at most, gravity_radius is less
//  than a chunk), from their block datas: on is block_data_active, as a door's open
static inline void gravity_sources_near(ecs* world, entity terrain, entity realm, int3 chunk_position, gravity_sources* sources) {
    sources->count = 0;
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const BlockLinks* blocks = zox_get(realm, BlockLinks);
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    int3 o;
    for (o.x = -1; o.x <= 1; o.x++) for (o.y = -1; o.y <= 1; o.y++) for (o.z = -1; o.z <= 1; o.z++) {
        const int3 position = int3_add(chunk_position, o);
        const entity chunk = int3_hashmap_get(chunks->value, position);
        if (!zox_valid(chunk) || !zox_has(chunk, BlockDatas) || zox_getv(chunk, NodeDepth) != depth) {
            continue;
        }
        const BlockDatas* datas = zox_get(chunk, BlockDatas);
        const VoxelNode* voxels = zox_get(chunk, VoxelNode);
        for (int i = 0; i + block_data_header <= datas->length; i += block_data_header + datas->value[i + 3]) {
            const byte* entry = &datas->value[i];
            if (!entry[3] || !(entry[block_data_header] & block_data_active)) {
                continue;
            }
            const byte3 local = { entry[0], entry[1], entry[2] };
            const byte voxel = getv_VoxelNode(voxels, depth, local);
            if (!voxel || voxel > blocks->length || !zox_has(blocks->value[voxel - 1], BlockGravity)) {
                continue;
            }
            if (sources->count < gravity_sources_max) {
                sources->block[sources->count++] = (int3) { position.x * length + local.x, position.y * length + local.y,
                    position.z * length + local.z };
            }
        }
    }
}

// works out a chunk's field again from the cubes switched on near it: each one's sphere flips it (overlaps flip back)
//  - full detail chunks only: a lower lod's voxels are bigger, nothing's pulled there (no bodies that far)
void gravity_rebuild(ecs* world, entity chunk) {
    const entity terrain = zox_get_parent(world, chunk);
    const entity realm = zox_valid(terrain) ? zox_get_parent(world, terrain) : 0;
    if (!zox_valid(realm) || !zox_has(realm, BlockLinks) || !zox_has(chunk, GravityOctree)) {
        return;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    zox_muter(chunk, GravityOctree, field);
    collapse_GravityOctree(field);
    field->value = gravity_default;
    if (zox_getv(chunk, NodeDepth) != depth) {
        return;
    }
    const int3 chunk_position = zox_getv(chunk, ChunkPosition);
    gravity_sources sources;
    gravity_sources_near(world, terrain, realm, chunk_position, &sources);
    if (!sources.count) {
        return;
    }
    const int length = octree_size(depth);
    const int r = gravity_radius;
    byte flips[length * length * length];
    memset(flips, 0, sizeof(flips));
    for (int s = 0; s < sources.count; s++) {
        const int3 c = {
            sources.block[s].x - chunk_position.x * length,
            sources.block[s].y - chunk_position.y * length,
            sources.block[s].z - chunk_position.z * length };
        for (int x = int_max(0, c.x - r); x <= int_min(length - 1, c.x + r); x++)
        for (int y = int_max(0, c.y - r); y <= int_min(length - 1, c.y + r); y++)
        for (int z = int_max(0, c.z - r); z <= int_min(length - 1, c.z + r); z++) {
            const int dx = x - c.x, dy = y - c.y, dz = z - c.z;
            if (dx * dx + dy * dy + dz * dz <= r * r) {
                flips[(x * length + y) * length + z] ^= 1;
            }
        }
    }
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        if (flips[(x * length + y) * length + z]) {
            set_GravityOctree(field, depth, (byte3) { x, y, z }, gravity_up);
        }
    }
    reduce_GravityOctree(field);
}

// a chunk's field to work out again, at the next merge (on_gravity_dirty); from a worker too (deferred)
static inline void gravity_dirty(ecs* world, entity chunk) {
    if (zox_valid(chunk) && zox_has(chunk, GravityOctree)) {
        zox_add(chunk, GravityDirty);
    }
}

// the chunks a gravity cube's sphere reaches, at block (toggled, placed or removed): their fields again
void gravity_dirty_around(ecs* world, entity terrain, int3 block) {
    if (!zox_valid(terrain) || !zox_has(terrain, ChunkLinks)) {
        return;
    }
    const byte depth = zox_getv(terrain, NodeDepth);
    const int3 r = int3_single(gravity_radius);
    const int3 lower = block_position_to_chunk_position(int3_sub(block, r), depth);
    const int3 upper = block_position_to_chunk_position(int3_add(block, r), depth);
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    int3 p;
    for (p.x = lower.x; p.x <= upper.x; p.x++) for (p.y = lower.y; p.y <= upper.y; p.y++) for (p.z = lower.z; p.z <= upper.z; p.z++) {
        gravity_dirty(world, int3_hashmap_get(chunks->value, p));
    }
}

// a chunk's voxels changed (VoxelNodeDirty: generated, loaded, edited), on the main thread
//  - a gravity cube switched on and removed or replaced (in its queue's edits, with their old voxel and data): the
//    chunks round it again
//  - cubes switched on in it or round it (loaded with it, or near where it came in): it again, and if they're in it,
//    the chunks they reach (a chunk loaded with one)
//  - none, and its field isn't empty: cleared
void on_gravity_voxels(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        const entity terrain = zox_get_parent(world, e);
        const entity realm = zox_valid(terrain) ? zox_get_parent(world, terrain) : 0;
        if (!zox_valid(realm) || !zox_has(realm, BlockLinks) || !zox_has(terrain, ChunkLinks)) {
            continue;
        }
        const int3 chunk_position = zox_getv(e, ChunkPosition);
        const int length = octree_size(zox_getv(terrain, NodeDepth));
        if (zox_has(e, VoxelNodeQueue)) {
            const VoxelNodeQueue* queue = zox_get(e, VoxelNodeQueue);
            const BlockLinks* blocks = zox_get(realm, BlockLinks);
            for (int j = 0; j < queue->count; j++) {
                const VoxelNodeUpdate* update = &queue->ptr[j];
                const byte old = update->old_value;
                if (update->state != zox_voxel_queue_post || !(update->old_data & block_data_active) || !old ||
                    old > blocks->length || !zox_has(blocks->value[old - 1], BlockGravity)) {
                    continue;
                }
                gravity_dirty_around(world, terrain, (int3) { chunk_position.x * length + update->position.x,
                    chunk_position.y * length + update->position.y, chunk_position.z * length + update->position.z });
            }
        }
        gravity_sources sources;
        gravity_sources_near(world, terrain, realm, chunk_position, &sources);
        const GravityOctree* field = zox_get(e, GravityOctree);
        if (sources.count || field->value != gravity_default || has_children_GravityOctree(field)) {
            gravity_dirty(world, e);
        }
        for (int s = 0; s < sources.count; s++) {
            if (int3_equals(block_position_to_chunk_position(sources.block[s], zox_getv(terrain, NodeDepth)), chunk_position)) {
                gravity_dirty_around(world, terrain, sources.block[s]);
            }
        }
    }
}

// GravityDirty: the field worked out again, then the event's gone
void on_gravity_dirty(iter* it) {
    zox_sys_world();
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        gravity_rebuild(world, e);
        zox_remove(e, GravityDirty);
    }
}
