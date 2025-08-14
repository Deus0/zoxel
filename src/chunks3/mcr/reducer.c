// =======================================
// zox_node_reducer.h
// Generic Octree Reducer (ptr-first structs, collapses uniform children)
// =======================================

// Core reducer: goes down to target depth, then merges children if uniform
static inline byte reduce_octree_node(
    void* node,
    byte target_depth,
    byte depth,
    size_t stride,
    size_t value_offset
) {
    if (!node) return 0;

    void** ptr = (void**)node;
    void* kids = *ptr;

    // If no children, nothing to reduce
    if (!kids) return 0;

    // Go deeper unless we are one step above target depth
    if (depth < target_depth - 1) {
        for (byte i = 0; i < 8; i++) {
            reduce_octree_node((char*)kids + i * stride, target_depth, depth + 1, stride, value_offset);
        }
    }

    // Check if all children have the same value and no sub-children
    byte first_val = *(byte*)((char*)kids + value_offset);
    bool uniform = true;
    for (byte i = 0; i < 8; i++) {
        void* child = (char*)kids + i * stride;
        byte val = *(byte*)((char*)child + value_offset);
        void* subkids = *(void**)child;
        if (val != first_val || subkids != NULL) {
            uniform = false;
            break;
        }
    }

    // Collapse if uniform
    if (uniform) {
        // Set parent’s value
        *(byte*)((char*)node + value_offset) = first_val;

        // Free children
        free(kids);
        *ptr = NULL;

        return 1;
    }

    return 0;
}

// Macro wrapper: type-safe reducer
#define create_node_reducer(T) \
static inline byte reduce_##T##_ex(T* node, byte target_depth, byte depth) { \
    return reduce_octree_node((void*)node, target_depth, depth, sizeof(T), offsetof(T, value)); \
}

// Example usage:
// create_node_reducer(LightNode)
// reduce_LightNode_ex(lroot, 5, 0);
