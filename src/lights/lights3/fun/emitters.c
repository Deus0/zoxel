// Emitters: blocks that give off light (BlockLight, a glow stone), lit from their sides
//  - an emitter is solid, so the flood skips its own voxel: it lights the air on its sides a step down
//  - placed (sys/editing.c), in a chunk generated or loaded (sys/emitters.c), and again when darkness floods past it
//    (sys/dark.c: covering a hole let the sun's dark take its light, nothing lit it back)

// the light on an emitter's sides
static inline byte emitter_side_light(byte emitted) {
    return emitted > light_air_decay ? emitted - light_air_decay : darklight;
}

// which chunk a position from this one is in: -1 itself, a side (direction_*) a step past one face, -2 further
//  - position wrapped into that chunk
static inline int emitter_chunk_side(int3* p, int length) {
    int side = -1;
    int outside = 0;
    const int axes[3] = { p->x, p->y, p->z };
    for (byte axis = 0; axis < 3; axis++) {
        const int v = axes[axis];
        if (v < 0 || v >= length) {
            if (v < -length || v >= 2 * length) {
                return -2;
            }
            side = axis * 2 + (v >= length);
            outside++;
        }
    }
    if (outside > 1) {
        return -2;
    }
    *p = (int3) { (p->x + length) % length, (p->y + length) % length, (p->z + length) % length };
    return side;
}

// a light or dark update at a position from this chunk, in it or the chunk a side over (a lamp's head, an emitter's
//  side past its edge), locked: other chunks' floods write these queues too
static inline void emitter_queue_light(LightQueue* own, LightQueue** sides, int3 p, int length, LightUpdate update) {
    if (zox_no_emitters) {
        return;
    }
    const int side = emitter_chunk_side(&p, length);
    LightQueue* queue = side == -1 ? own : side >= 0 && sides ? sides[side] : NULL;
    if (!queue) {
        return;
    }
    update.pos = int3_to_byte3(p);
    spin_lock(&queue->lock);
    a_LightQueue(queue, update);
    spin_unlock(&queue->lock);
    __atomic_add_fetch(&light_emitter_queued, 1, __ATOMIC_RELAXED);
}

static inline void emitter_queue_dark(DarkQueue* own, DarkQueue** sides, int3 p, int length, DarkUpdate update) {
    const int side = emitter_chunk_side(&p, length);
    DarkQueue* queue = side == -1 ? own : side >= 0 && sides ? sides[side] : NULL;
    if (!queue) {
        return;
    }
    update.pos = int3_to_byte3(p);
    spin_lock(&queue->lock);
    a_DarkQueue(queue, update);
    spin_unlock(&queue->lock);
}

// where an emitter gives off its light: its voxel plus its offset, turned to its direction (its block data)
static inline int3 emitter_position(byte3 position, byte3 offset, byte data) {
    const int3 turned = block_direction_offset(block_data_direction(data), byte3_to_int3(offset));
    return (int3) { position.x + turned.x, position.y + turned.y, position.z + turned.z };
}

// an emitter at position lights its sides (from its offset: a street lamp's head)
static inline void emitter_light_sides(LightQueue* own, LightQueue** sides, byte3 position, byte3 offset, byte data,
    int length, byte depth, byte emitted)
{
    const int3 at = emitter_position(position, offset, data);
    for (byte d = 0; d < 6; d++) {
        const sbyte3 o = neighbor_offsets[d];
        emitter_queue_light(own, sides, (int3) { at.x + o.x, at.y + o.y, at.z + o.z }, length, (LightUpdate) {
            .depth = depth,
            .light = emitter_side_light(emitted),
            .distance = light_propogation_distance,
            .channel = zox_light_block });      // blocks' light, its own tree
    }
}

// the light each block gives off from its own voxel, 0 for one that gives it off from an offset (a lamp's base: the
//  dark meeting it can't tell where its head is)
static inline void emission_from_voxel(const byte* emission, const byte3* emission_offset, byte* glows) {
    for (int j = 0; j < zox_light_solidity_length; j++) {
        const byte3 o = emission_offset[j];
        glows[j] = !o.x && !o.y && !o.z ? emission[j] : 0;
    }
}

// every emitter in a chunk's voxels lights its sides, walking the tree (closed nodes of air skipped whole)
static void emitters_light_node(const VoxelNode* node, const BlockDatas* datas, byte node_depth, byte depth,
    byte3 position, const byte* emission, const byte3* emission_offset, LightQueue* own, LightQueue** sides, uint* count)
{
    const VoxelNode* kids = node_depth < depth ? get_children_VoxelNode(node) : NULL;
    if (kids) {
        for (byte i = 0; i < octree_length; i++) {
            const byte3 o = octree_positions[i];
            emitters_light_node(&kids[i], datas, node_depth + 1, depth,
                (byte3) { position.x * 2 + o.x, position.y * 2 + o.y, position.z * 2 + o.z },
                emission, emission_offset, own, sides, count);
        }
        return;
    }
    const byte voxel = node->value;
    if (!voxel || !emission[voxel - 1]) {
        return;
    }
    // a closed node above the depth: a cube of them, each one lights (most are inside it, their sides solid)
    const int length = octree_size(depth);
    const byte span = 1 << (depth - node_depth);
    for (byte x = 0; x < span; x++) for (byte y = 0; y < span; y++) for (byte z = 0; z < span; z++) {
        const byte3 p = { position.x * span + x, position.y * span + y, position.z * span + z };
        const byte data = datas ? get_block_data(datas, p) : 0;
        emitter_light_sides(own, sides, p, emission_offset[voxel - 1], data, length, depth, emission[voxel - 1]);
        (*count)++;
    }
}

static inline uint emitters_light_chunk(const VoxelNode* voxels, const BlockDatas* datas, byte depth,
    const byte* emission, const byte3* emission_offset, LightQueue* own, LightQueue** sides)
{
    uint count = 0;
    if (voxels) {
        emitters_light_node(voxels, datas, 0, depth, byte3_zero, emission, emission_offset, own, sides, &count);
    }
    return count;
}
