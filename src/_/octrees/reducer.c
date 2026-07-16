// =======================================
// zox_node_reducer.h
// Generic Octree Reducer (ptr-first structs, collapses uniform children)
// =======================================
static inline byte reduce_octree_node(void* node, size_t stride, size_t value_offset, size_t type_offset) {
    if (!node) {
        return 0;
    }
    void** ptr = (void**) node;
    if (type_offset) {
        byte type = *(byte*)((char*)node + type_offset);
        if (type == node_type_instance) {
            return 0;
        }
    }
    void* kids = *ptr;
    // No children, nothing to reduce
    if (!kids) {
        return 0;
    }
    // Recurse bottom-up
    for (byte i = 0; i < 8; i++) {
        reduce_octree_node((char*) kids + i * stride, stride, value_offset, type_offset);
    }
    // Check uniformity
    byte first_val = *(byte*)((char*) kids + value_offset);
    bool uniform = true;
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        // NOTE: Don't collapse nodes with entity links
        if (type_offset) {
            byte type = *(byte*)((char*)child + type_offset);
            if (type == node_type_instance) {
                uniform = 0;
                break;
            }
        }
        byte val = *(byte*)((char*) child + value_offset);
        void* subkids = *(void**) child;
        if (val != first_val || subkids != NULL) {
            uniform = 0;
            break;
        }
    }
    // Collapse if uniform
    if (uniform) {
        *(byte*)((char*) node + value_offset) = first_val;
        free(kids);
        *ptr = NULL;
        return 1;
    }
    return 0;
}

// Macro wrapper: type-safe reducer
#define create_octree_reducer(T) \
static inline byte reduce_##T(T* node) { \
    return reduce_octree_node((void*) node, sizeof(T), offsetof(T, value), 0); \
}

#define create_octree_reducer_linked(T) \
static inline byte reduce_##T(T* node) { \
    return reduce_octree_node((void*) node, sizeof(T), offsetof(T, value), offsetof(T, type)); \
}

// Usage:
// create_node_reducer(LightNode)
// reduce_LightNode(lroot);
