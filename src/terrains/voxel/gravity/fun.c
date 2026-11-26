// a field's value: 0 the realm's own gravity (zox_default_gravity, down), else where it pulls, 1 + axis * 2 + (1 if
//  it pulls along +axis): 1 -x, 2 +x, 3 -y, 4 +y, 5 -z, 6 +z
#define gravity_default 0
#define gravity_up 4            // where a gravity cube with no way of its own pulls (one placed before they had 6)
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
    byte pull[gravity_sources_max];     // the way it pulls (its block data, block_pointing), the field's value
} gravity_sources;

typedef struct {
    ecs* world;
    const VoxelNode* voxels;
    const BlockDatas* datas;
    const BlockLinks* blocks;
    byte depth;
    int3 origin;    // the chunk's first block
    gravity_sources* sources;
} gravity_source_search;

// the voxels on under a node (at node_depth, position in its units) that are gravity cubes
static void gravity_sources_node(const gravity_source_search* search, const ActiveOctree* node, byte node_depth, byte3 position) {
    ecs* world = search->world;
    const ActiveOctree* kids = get_children_ActiveOctree(node);
    if (kids) {
        for (byte i = 0; i < octree_length; i++) {
            const byte3 o = octree_positions[i];
            gravity_sources_node(search, &kids[i], node_depth + 1,
                (byte3) { position.x * 2 + o.x, position.y * 2 + o.y, position.z * 2 + o.z });
        }
        return;
    }
    if (!node->value) {
        return;
    }
    const byte span = 1 << (search->depth - node_depth);
    for (byte x = 0; x < span; x++) for (byte y = 0; y < span; y++) for (byte z = 0; z < span; z++) {
        const byte3 local = { position.x * span + x, position.y * span + y, position.z * span + z };
        const byte voxel = getv_VoxelNode(search->voxels, search->depth, local);
        gravity_sources* sources = search->sources;
        if (voxel && voxel <= search->blocks->length && zox_has(search->blocks->value[voxel - 1], BlockGravity) &&
            sources->count < gravity_sources_max) {
            const byte way = search->datas ? get_block_data(search->datas, local) : 0;
            sources->pull[sources->count] = way >= 1 && way <= 6 ? way : gravity_up;
            sources->block[sources->count++] = int3_add(search->origin, byte3_to_int3(local));
        }
    }
}

// the gravity cubes switched on in a chunk and the 26 round it (they reach one chunk at most, gravity_radius is less
//  than a chunk), from their ActiveOctrees: a chunk with nothing on is its root, 0, skipped
static inline void gravity_sources_near(ecs* world, entity terrain, entity realm, int3 chunk_position, gravity_sources* sources) {
    sources->count = 0;
    const ChunkLinks* chunks = zox_get(terrain, ChunkLinks);
    const byte depth = zox_getv(terrain, NodeDepth);
    const int length = octree_size(depth);
    int3 o;
    for (o.x = -1; o.x <= 1; o.x++) for (o.y = -1; o.y <= 1; o.y++) for (o.z = -1; o.z <= 1; o.z++) {
        const int3 position = int3_add(chunk_position, o);
        const entity chunk = int3_hashmap_get(chunks->value, position);
        if (!zox_valid(chunk) || !zox_has(chunk, ActiveOctree) || zox_getv(chunk, NodeDepth) != depth) {
            continue;
        }
        const ActiveOctree* actives = zox_get(chunk, ActiveOctree);
        if (!active_octree_any(actives)) {
            continue;
        }
        const gravity_source_search search = { world, zox_get(chunk, VoxelNode),
            zox_has(chunk, BlockDatas) ? zox_get(chunk, BlockDatas) : NULL, zox_get(realm, BlockLinks), depth,
            int3_scale(position, length), sources };
        gravity_sources_node(&search, actives, 0, byte3_zero);
    }
}

// works out a chunk's field again from the cubes switched on near it: each one's sphere pulls its way, the nearest
//  cube's where they overlap (the first of those as near)
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
    const int count = length * length * length;
    byte pulls[count];
    int nearest[count];
    memset(pulls, 0, sizeof(pulls));
    for (int s = 0; s < sources.count; s++) {
        const int3 c = {
            sources.block[s].x - chunk_position.x * length,
            sources.block[s].y - chunk_position.y * length,
            sources.block[s].z - chunk_position.z * length };
        for (int x = int_max(0, c.x - r); x <= int_min(length - 1, c.x + r); x++)
        for (int y = int_max(0, c.y - r); y <= int_min(length - 1, c.y + r); y++)
        for (int z = int_max(0, c.z - r); z <= int_min(length - 1, c.z + r); z++) {
            const int dx = x - c.x, dy = y - c.y, dz = z - c.z;
            const int d2 = dx * dx + dy * dy + dz * dz;
            const int i = (x * length + y) * length + z;
            if (d2 <= r * r && (!pulls[i] || d2 < nearest[i])) {
                pulls[i] = sources.pull[s];
                nearest[i] = d2;
            }
        }
    }
    for (int x = 0; x < length; x++) for (int y = 0; y < length; y++) for (int z = 0; z < length; z++) {
        const byte pull = pulls[(x * length + y) * length + z];
        if (pull) {
            set_GravityOctree(field, depth, (byte3) { x, y, z }, pull);
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
//  - a gravity cube switched on and removed or replaced (in its queue's edits, their old voxel and old_active): the
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
                if (update->state != zox_voxel_queue_post || !update->old_active || !old ||
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
