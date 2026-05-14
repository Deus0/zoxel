// =======================================
// zox_node_setter.h
// Generic Octree Setter (ptr-first structs, opens nodes)
// =======================================

// Core setter: walks toward target depth, sets value, opens children if missing
static inline void* set_octree_value(void* node, byte tdepth, byte3 pos, byte value, byte depth, size_t stride, size_t value_offset) {
    if (!node) {
        return NULL;
    }
    if (depth == 0) {
        // check bounds
        byte length = powers_of_two[tdepth];
        if (pos.x >= length || pos.y >= length || pos.z >= length) {
            zox_logw("OOB [set_octree_value] [%ix%ix%i] depth [%i] vlength [%i]", pos.x, pos.y, pos.z, tdepth, length);
            return NULL;
        }
    }
    // Are we at target depth?
    byte depth_reached = (depth == tdepth);
    void** ptr = (void**) node;
    void* kids = *ptr;
    // Open children if missing and we need to go deeper
    if (!depth_reached && !kids) {
        kids = malloc(stride * 8);   // allocate 8 children
        if (!kids) {
            zox_loge("[set_octree_value] Allocation Failure");
            return node;
        }
        memset(kids, 0, stride * 8);    // zero-init
        *ptr = kids;
        // --- New: set all children values ---
        byte parent_value = *(byte*)((char*) node + value_offset);
        for (byte j = 0; j < 8; j++) {
            void* child = (char*)(kids) + j * stride;
            *(byte*)((char*)child + value_offset) = parent_value;
        }
    }
    // Set value if reached depth
    if (depth_reached) {
        // Pointer math to set value
        *(byte*)((char*) node + value_offset) = value;
        // zox_log("Depth [%i] Reached [%i]", tdepth, value);
        return node;
    }
    // Dive into correct child
    byte div = powers_of_two_byte[tdepth - depth - 1];
    if (!div) {
        return node;
    }
    byte3 npos = { pos.x / div, pos.y / div, pos.z / div };
    byte i = byte3_octree_array_index(npos);
    if (i >= 8) {
        zox_logw("[set_octree_value] Invalid Index >= 8 [%i]\n  - pos [%ix%ix%i]\n    - npos [%ix%ix%i]\n    - div [%i]\n    - depth [%i]\n    - tdepth [%i]", i,
            pos.x, pos.y, pos.z,
            npos.x, npos.y, npos.z,
            div, depth, tdepth);
        return node;
    }
    byte3 cpos = { pos.x % div, pos.y % div, pos.z % div };
    return set_octree_value((char*) kids + i * stride, tdepth, cpos, value, depth + 1, stride, value_offset);
}

// Macro wrapper: type-safe setter
#define create_node_setter(T) \
\
static inline T* set_##T(T* node, byte tdepth, byte3 pos, byte value, byte depth) { \
    return (T*) set_octree_value(\
        (void*) node,\
        tdepth,\
        pos, value,\
        depth,\
        sizeof(T),\
        offsetof(T, value)\
    ); \
}

// Example usage:
// create_node_setter(VoxelNode)
// create_node_setter(LightNode)
// set_VoxelNode(vroot, 5, (byte3){x,y,z}, 1, 0);
// set_LightNode(lroot, 5, (byte3){x,y,z}, 255, 0);
