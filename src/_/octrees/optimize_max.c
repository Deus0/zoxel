// NOTE: Optimizes octree by picking highest value
static inline void reduce_to_max_octree_node(void* node, size_t stride, size_t value_offset, size_t type_offset) {
    if (!node) {
        return;
    }
    void** ptr = (void**) node;
    void* kids = *ptr;
    // Leaf node, nothing to optimize
    if (!kids) {
        return;
    }
    if (type_offset) {
        byte type = *(byte*)((char*) node + type_offset);
        if (type == node_type_instance) {
            return;
        }
    }
    // Recurse bottom-up
    for (byte i = 0; i < 8; i++) {
        reduce_to_max_octree_node((char*) kids + i * stride, stride, value_offset, type_offset);
    }
    // Find highest child value
    byte max_val = 0;
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        byte value = *(byte*)((char*) child + value_offset);
        // skip air if VoxelOctree
        if (type_offset && !value) {
            continue;
        }
        if (value > max_val) {
            max_val = value;
        }
    }
    // Set parent value
    *(byte*)((char*) node + value_offset) = max_val;
}

// Macro wrapper
#define create_optimize_by_max(T) \
static inline void optimize_by_max_##T(T* node) { \
    reduce_to_max_octree_node((void*) node, sizeof(T), offsetof(T, value), 0); \
}

// Usage:
// create_optimize_by_max(LightNode);
// optimize_by_max_LightNode(lroot);