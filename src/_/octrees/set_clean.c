// =======================================
// zox_node_setter.h
// Generic Octree Setter (ptr-first structs, opens nodes)
// =======================================

// NOTE: Like set_octree_value but sets new nodes to Air!
/*static inline byte set_octree_value_clean(void* node, byte tdepth, byte3 pos, byte value, byte depth, size_t stride, size_t value_offset) {
    if (!node) {
        return 0;
    }
    if (depth == 0) {
        // check bounds
        short length = octree_size(tdepth);
        if (pos.x >= length || pos.y >= length || pos.z >= length) {
            if (dbg_log_octree_errors) {
                zox_logw("OOB [set_octree_value_clean] [%ix%ix%i] depth [%i] vlength [%i]", pos.x, pos.y, pos.z, tdepth, length);
            }
            return 0;
        }
    }
    void** ptr = (void**) node;
    void* kids = *ptr;
    // Open children if missing and we need to go deeper
    byte depth_reached = depth == tdepth;
    if (!depth_reached && !kids) {
        kids = malloc(stride * 8);   // allocate 8 children
        if (!kids) {
            zox_loge("[set_octree_value_clean] Allocation Failure");
            return 0;
        }
        memset(kids, 0, stride * 8);    // zero-init
        *ptr = kids;
    }
    // Set value if reached depth
    if (depth_reached) {
        // Pointer math to set value
        *(byte*)((char*) node + value_offset) = value;
        // zox_log("Depth [%i] Reached [%i]", tdepth, value);
        return 1;
    }
    // Dive into correct child
    short div = octree_size(tdepth - depth - 1);
    if (!div) {
        return 0;
    }
    byte3 npos = { pos.x / div, pos.y / div, pos.z / div };
    byte i = byte3_octree_array_index(npos);
    if (i >= 8) {
        zox_logw("[set_octree_value_clean] Invalid Index >= 8 [%i]\n  - pos [%ix%ix%i]\n    - npos [%ix%ix%i]\n    - div [%i]\n    - depth [%i]\n    - tdepth [%i]", i, pos.x, pos.y, pos.z, npos.x, npos.y, npos.z, div, depth, tdepth);
        return 0;
    }
    byte3 cpos = { pos.x % div, pos.y % div, pos.z % div };
    return set_octree_value_clean((char*) kids + i * stride, tdepth, cpos, value, depth + 1, stride, value_offset);
}

static inline byte set_octree_value_clean2(void *node, byte tdepth, byte3 pos, byte value, size_t stride, size_t value_offset) {
    if (!node) {
        return 0;
    }
    short length = octree_size(tdepth);
    if (pos.x >= length || pos.y >= length || pos.z >= length) {
        if (dbg_log_octree_errors) {
            zox_logw("OOB [set_octree_value_clean2] [%ix%ix%i] depth [%i] vlength [%i]",
                     pos.x, pos.y, pos.z, tdepth, length);
        }
        return 0;
    }
    byte depth = 0;
    while (depth < tdepth) {
        void **ptr = (void **) node;
        void *kids = *ptr;
        if (!kids) {
            kids = calloc(8, stride);
            if (!kids) {
                zox_loge("[set_octree_value_clean2] Allocation Failure");
                return 0;
            }
            *ptr = kids;
        }
        byte shift = tdepth - depth - 1;
        byte mask = (1 << shift) - 1;
        byte3 npos = {
            pos.x >> shift,
            pos.y >> shift,
            pos.z >> shift
        };
        byte i = byte3_octree_array_index(npos);
#ifdef zox_safety_checks
        if (i >= 8) {
            zox_loge("[set_octree_value_clean2] Invalid Index [%i]", i);
            return 0;
        }
#endif
        node = (char *) kids + i * stride;
        pos.x &= mask;
        pos.y &= mask;
        pos.z &= mask;
        depth++;
    }
    *(byte *)((char *)node + value_offset) = value;
    return 1;
}
*/
