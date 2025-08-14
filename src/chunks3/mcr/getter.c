// =======================================
// zox_node_getter.h
// Generic Octree Getter (ptr-first structs)
// =======================================

// Core: walk toward target depth, return deepest reachable node
static inline const void* find_octree_node(
    const void* node,
    byte target_depth,
    byte3 pos,
    byte depth,
    size_t stride
) {
    while (node && depth < target_depth) {
        void* const* kids = (void* const*)node;   // first member = child array
        if (!*kids) return node;                  // stop if children missing

        const byte div = powers_of_two_byte[target_depth - depth - 1];
        if (div == 0) break;

        byte3 node_pos = { pos.x / div, pos.y / div, pos.z / div };
        byte3_modulus_byte(&pos, div);

        byte i = byte3_octree_array_index(node_pos);
        if (i >= 8) return NULL;

        node = (char*)(*kids) + i * stride;
        depth++;
    }
    return node;
}

// Core: read node value at deepest reachable node
static inline byte read_octree_value(const void* node, byte target_depth, byte3 pos, byte depth, size_t stride, size_t value_offset) {
    node = find_octree_node(node, target_depth, pos, depth, stride);
    if (!node) return 0;
    return *(byte*)((char*)node + value_offset);
}

// Macro wrapper: generates type-safe getters
#define create_node_getter(T)                                             \
static inline const T* get_##T##_ex(const T* node, byte target_depth, byte3 pos, byte depth) { \
    return (T*)find_octree_node((const void*)node, target_depth, pos, depth, sizeof(T)); \
}                                                                         \
static inline byte get_##T##_value_ex(const T* node, byte target_depth, byte3 pos, byte depth) { \
    return read_octree_value((void*)node, target_depth, pos, depth, sizeof(T), offsetof(T, value)); \
}

// Example usage:
// create_node_getter(VoxelNode)
// create_node_getter(LightNode)
// VoxelNode* n = get_VoxelNode_ex(root, target_depth, (byte3){x,y,z}, 0);
// byte v = get_VoxelNode_value_ex(root, target_depth, (byte3){x,y,z}, 0);

