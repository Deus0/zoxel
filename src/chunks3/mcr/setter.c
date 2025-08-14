// =======================================
// zox_node_setter.h
// Generic Octree Setter (ptr-first structs, opens nodes)
// =======================================

// Core setter: walks toward target depth, sets value, opens children if missing
static inline void* set_octree_value(void* node, byte target_depth, byte3 pos, byte value, byte depth, size_t stride, size_t value_offset) {
    if (!node) return NULL;

    // Are we at target depth?
    bool depth_reached = (depth == target_depth);

    // Set value if needed
    if (depth_reached || value) {
        *(byte*)((char*)node + value_offset) = value;
    }

    // Children ptr is first member
    void** ptr = (void**)node;

    // Open children if missing and we need to go deeper
    if (!depth_reached && !*ptr) {
        *ptr = malloc(stride * 8);   // allocate 8 children
        if (!*ptr) {
            zox_log_error("[set_octree_value] failed to allocate children");
            return node;
        }
        memset(*ptr, 0, stride * 8);    // zero-init
    }

    void* kids = *ptr;
    if (depth_reached || !kids) return node;

    // Dive into correct child
    const byte div = powers_of_two_byte[target_depth - depth - 1];
    if (div == 0) return node;

    byte3 node_pos = { pos.x / div, pos.y / div, pos.z / div };
    byte3_modulus_byte(&pos, div);

    byte i = byte3_octree_array_index(node_pos);
    if (i >= 8) {
        zox_log_error("[set_octree_value] index OOB: %u", (unsigned)i);
        return node;
    }

    return set_octree_value((char*)kids + i * stride, target_depth, pos, value, depth + 1, stride, value_offset);
}

// Macro wrapper: type-safe setter
#define create_node_setter(T) \
static inline T* set_##T##_ex(T* node, byte target_depth, byte3 pos, byte value, byte depth) { \
    return (T*)set_octree_value((void*)node, target_depth, pos, value, depth, sizeof(T), offsetof(T, value)); \
}

// Example usage:
// create_node_setter(VoxelNode)
// create_node_setter(LightNode)
// set_VoxelNode_ex(vroot, 5, (byte3){x,y,z}, 1, 0);
// set_LightNode_ex(lroot, 5, (byte3){x,y,z}, 255, 0);
