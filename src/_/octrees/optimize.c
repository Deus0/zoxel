// NOTE: Optimizes octree by picking most popular value
static inline void optimize_octree_node(void* node, size_t stride, size_t value_offset, size_t type_offset) {
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
        optimize_octree_node((char*) kids + i * stride, stride, value_offset, type_offset);
    }
    // Count child values (max 8)
    byte child_vals[8];
    byte child_count = 0;
    byte child_counts[8] = { 0 };
    // Track unique values and counts
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        byte value = *(byte*)((char*) child + value_offset);
        byte j;
        for (j = 0; j < child_count; j++) {
            if (child_vals[j] == value) {
                break;
            }
        }
        if (j == child_count) {
            child_vals[child_count] = value;
            child_count++;
        }
    }
    // Simple frequency array for mode (up to 8 values)
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        byte value = *(byte*)((char*) child + value_offset);
        for (byte j = 0; j < child_count; j++) {
            if (child_vals[j] == value) {
                child_counts[j]++;
                break;
            }
        }
    }
    // Find mode
    byte max_count = 0;
    byte mode_val = 0;
    for (byte i = 0; i < child_count; i++) {
        byte value = child_vals[i];
        byte count = child_counts[i];
        // skip air if VoxelOctree
        if (type_offset && !value) {
            continue;
        }
        if (count > max_count) {
            max_count = count;
            mode_val = value;
        }
    }
    // Set parent value
    *(byte*)((char*) node + value_offset) = mode_val;
}

// Macro wrapper
#define create_octree_optimizer(T) \
static inline void optimize_##T(T* node) { \
    optimize_octree_node((void*) node, sizeof(T), offsetof(T, value), 0); \
}

#define create_octree_optimizer_linked(T) \
static inline void optimize_##T(T* node) { \
    optimize_octree_node((void*) node, sizeof(T), offsetof(T, value), offsetof(T, type)); \
}

// Usage:
// create_node_optimizer(LightNode)
// optimize_LightNode(lroot);
