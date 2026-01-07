// =======================================
// zox_node_reducer.h
// Generic Octree Reducer (ptr-first structs, collapses uniform children)
// =======================================

static inline byte reduce_octree_node(void* node, size_t stride, size_t value_offset) {
    if (!node) return 0;

    void** ptr = (void**)node;
    void* kids = *ptr;

    // No children, nothing to reduce
    if (!kids) return 0;

    // Recurse bottom-up
    for (byte i = 0; i < 8; i++) {
        reduce_octree_node((char*)kids + i * stride, stride, value_offset);
    }

    // Check uniformity
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
        *(byte*)((char*)node + value_offset) = first_val;
        free(kids);
        *ptr = NULL;
        return 1;
    }

    return 0;
}

// Macro wrapper: type-safe reducer
#define create_node_reducer(T) \
static inline byte reduce_##T(T* node) { \
    return reduce_octree_node((void*)node, sizeof(T), offsetof(T, value)); \
}

// Usage:
// create_node_reducer(LightNode)
// reduce_LightNode(lroot);
