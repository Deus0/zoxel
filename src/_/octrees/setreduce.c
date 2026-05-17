// =======================================
// zox_node_setreduce.h
// Octree Setter + Immediate Branch Collapse
// =======================================
// Forward declaration of internal collapse check for one branch
static inline byte collapse_octree_branch(void* node, size_t stride, size_t value_offset) {
    if (!node) {
        return 0;
    }
    void** ptr = (void**)node;
    void* kids = *ptr;
    if (!kids) {
        return 0;
    }
    // Check if all children match first value and have no subchildren
    byte first_val = *(byte*)((char*)kids + value_offset);
    for (byte i = 0; i < 8; i++) {
        void* child = (char*)kids + i * stride;
        byte val = *(byte*)((char*)child + value_offset);
        void* subkids = *(void**)child;
        if (val != first_val || subkids != NULL) {
            return 0; // can't collapse
        }
    }
    // Collapse
    *(byte*)((char*)node + value_offset) = first_val;
    free(kids);
    *ptr = NULL;
    return 1;
}

// Core setter+collapse: same as set_octree_value but collapses upwards
static inline void* setreduce_octree_value(
    void* node, byte target_depth, byte3 pos, byte value, byte depth, size_t stride, size_t value_offset
) {
    if (!node)  {
        return NULL;
    }
    bool depth_reached = (depth == target_depth);
    if (depth_reached || value) {
        *(byte*)((char*)node + value_offset) = value;
    }
    void** ptr = (void**)node;
    if (!depth_reached && !*ptr) {
        *ptr = malloc(stride * 8);
        if (!*ptr)  {
            return node;
        }
        memset(*ptr, 0, stride * 8);
    }
    void* kids = *ptr;
    if (depth_reached || !kids) {
        // Try collapse if we just hit target
        collapse_octree_branch(node, stride, value_offset);
        return node;
    }
    byte div = powers_of_two_byte[target_depth - depth - 1];
    if (div == 0) {
        return node;
    }
    byte3 node_pos = { pos.x / div, pos.y / div, pos.z / div };
    byte3_modulus_byte(&pos, div);
    byte i = byte3_octree_array_index(node_pos);
    if (i >= 8) {
        return node;
    }
    setreduce_octree_value((char*)kids + i * stride, target_depth, pos, value, depth + 1, stride, value_offset);
    // After child update, check if we can collapse this node
    collapse_octree_branch(node, stride, value_offset);
    return node;
}

// Macro wrapper: type-safe setreduce
#define create_node_setreduce(T) \
static inline T* setreduce_##T##_ex(T* node, byte target_depth, byte3 pos, byte value, byte depth) { \
    return (T*)setreduce_octree_value((void*)node, target_depth, pos, value, depth, sizeof(T), offsetof(T, value)); \
}

// Example usage:
// create_node_setreduce(LightNode)
// setreduce_LightNode(lroot, 5, (byte3){x,y,z}, 0, 0);
