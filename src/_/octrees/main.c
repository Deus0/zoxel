// =======================================
// zox_node_getter.h
// Generic Octree Getter (ptr-first structs)
// =======================================

byte dbg_log_errors_get_octree_mut = 1;

// Core: walk toward target depth, return deepest reachable node
/*static inline const void* get_octree(const void* node, byte target_depth, byte3 pos, byte depth, size_t stride) {
    if (depth == 0) {
        // check bounds
        short length = octree_size(target_depth);
        if (pos.x >= length || pos.y >= length || pos.z >= length) {
            if (dbg_log_octree_errors) {
                zox_logw("OOB [get_octree] [%ix%ix%i] depth [%i] vlength [%i]", pos.x, pos.y, pos.z, target_depth, length);
            }
            return NULL;
        }
    }
    while (node && depth < target_depth) {
        // quick sanity: reject obviously bad pointers
        uintptr_t p = (uintptr_t) node;
        if (p == 0 || (p & 0x7) != 0) {
            return node;
        }
        // read the first-field pointer from the node
        const void* kids_ptr = *(const void**) node;
        if (!kids_ptr) {
            return node;
        }
        short div = octree_size(target_depth - depth - 1);
#ifdef zox_safety_checks
        if (div == 0) {
            zox_loge("Division is 0 in [get_octree]");
            break;
        }
#endif
        byte3 npos = { pos.x / div, pos.y / div, pos.z / div };
        byte3_modulus_byte(&pos, div);
        byte i = byte3_octree_array_index(npos);
#ifdef zox_safety_checks
        if (i >= 8) {
            zox_loge("[get_octree] Invalid Index >= 8 [%i]\n  - pos [%ix%ix%i]\n    - npos [%ix%ix%i]\n    - div [%i]\n    - depth [%i]\n    - target_depth [%i]", i,
                    pos.x, pos.y, pos.z,
                    npos.x, npos.y, npos.z,
                    div, depth, target_depth);
            return NULL;
        }
#endif
        node = (char*) kids_ptr + i * stride;
        depth++;
    }
    return node;
}*/

// Core: read node value at deepest reachable node
/*static inline byte read_octree_value(const void* node, byte target_depth, byte3 pos, byte depth, size_t stride, size_t value_offset) {
    node = get_octree(node, target_depth, pos, depth, stride);
    if (!node) {
        return 0;
    }
#ifdef zox_safety_checks
    uintptr_t p = (uintptr_t) node;
    if (p == 0 || (p & 0x7) != 0) {
        return 0;
    }
#endif
    return *(byte*)((char*) node + value_offset);
}*/


/*static inline const void *get_octree(
    const void *node,
    byte target_depth,
    byte3 pos,
    size_t stride)
{
    if (!node) {
        return NULL;
    }
    short length = octree_size(target_depth);
    if (pos.x >= length || pos.y >= length || pos.z >= length) {
        if (dbg_log_octree_errors) {
            zox_logw("OOB [get_octree] [%ix%ix%i] depth [%i] vlength [%i]",
                     pos.x, pos.y, pos.z, target_depth, length);
        }
        return NULL;
    }
    byte depth = 0;
    while (depth < target_depth) {
#ifdef zox_safety_checks
        uintptr_t p = (uintptr_t)node;
        if ((p & 7) != 0) {
            return node;
        }
#endif
        const void *kids = *(const void **)node;
        if (!kids) {
            return node;
        }
        byte shift = target_depth - depth - 1;
        byte mask = (1 << shift) - 1;
        byte3 npos = {
            pos.x >> shift,
            pos.y >> shift,
            pos.z >> shift
        };
        byte i = byte3_octree_array_index(npos);
#ifdef zox_safety_checks
        if (i >= 8) {
            zox_loge("[get_octree] Invalid Index [%i]", i);
            return NULL;
        }
#endif
        node = (const char *)kids + i * stride;
        pos.x &= mask;
        pos.y &= mask;
        pos.z &= mask;
        depth++;
    }
    return node;
}*/

/*static inline byte get_octree_value(
    const void *node,
    byte target_depth,
    byte3 pos,
    size_t stride,
    size_t value_offset)
{
    if (!node) {
        return 0;
    }
    short length = octree_size(target_depth);
    if (pos.x >= length || pos.y >= length || pos.z >= length) {
        return 0;
    }
    byte depth = 0;
    while (depth < target_depth) {
        const void *kids = *(const void **)node;
        if (!kids) {
            break;
        }
        byte shift = target_depth - depth - 1;
        byte mask = (1 << shift) - 1;
        byte i = byte3_octree_array_index((byte3){
            pos.x >> shift,
            pos.y >> shift,
            pos.z >> shift
        });
        node = (const char *)kids + i * stride;
        pos.x &= mask;
        pos.y &= mask;
        pos.z &= mask;
        depth++;
    }
    return *(const byte *)((const char *)node + value_offset);
}*/

// =======================================
// zox_node_setter.h
// Generic Octree Setter (ptr-first structs, opens nodes)
// =======================================

// Core setter: walks toward target depth, sets value, opens children if missing
/*static inline void* set_octree_value(void* node, byte tdepth, byte3 pos, byte value, byte depth, size_t stride, size_t value_offset) {
    if (!node) {
        return NULL;
    }
    if (depth == 0) {
        // check bounds
        short length = octree_size(tdepth);
        if (pos.x >= length || pos.y >= length || pos.z >= length) {
            if (dbg_log_octree_errors) {
                zox_logw("OOB [set_octree_value] [%ix%ix%i] depth [%i] vlength [%i]", pos.x, pos.y, pos.z, tdepth, length);
            }
            return NULL;
        }
    }
    void** ptr = (void**) node;
    void* kids = *ptr;
    // Open children if missing and we need to go deeper
    byte depth_reached = depth == tdepth;
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
    short div = octree_size(tdepth - depth - 1);
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
}*/

/*static inline void* set_octree_value2(
    void* node,
    byte tdepth,
    byte3 pos,
    byte value,
    byte inherit_parent_value,
    size_t stride,
    size_t value_offset)
{
    if (!node) {
        return NULL;
    }
    short length = octree_size(tdepth);
    if (pos.x >= length || pos.y >= length || pos.z >= length) {
        if (dbg_log_octree_errors) {
            zox_logw("OOB [set_octree_value2] [%ix%ix%i] depth [%i] vlength [%i]",
                     pos.x, pos.y, pos.z, tdepth, length);
        }
        return NULL;
    }
    byte depth = 0;
    while (depth < tdepth) {
        void **ptr = (void **)node;
        void *kids = *ptr;
        if (!kids) {
            kids = calloc(8, stride);
            if (!kids) {
                zox_loge("[set_octree_value2] Allocation Failure");
                return node;
            }
            *ptr = kids;
            if (inherit_parent_value) {
                byte parent_value =
                *(byte *)((char *)node + value_offset);
                char *child = (char *)kids + value_offset;
                for (byte j = 0; j < 8; j++) {
                    *(byte *)child = parent_value;
                    child += stride;
                }
            }
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
            zox_loge("[set_octree_value2] Invalid Index [%i]", i);
            return node;
        }
#endif
        node = (char *)kids + i * stride;
        pos.x &= mask;
        pos.y &= mask;
        pos.z &= mask;
        depth++;
    }
    *(byte *)((char *)node + value_offset) = value;
    return node;
}*/

// TODO: Just make all these start at 0 depth
/*static inline void* get_octree_mut(void* node, byte target_depth, byte3 pos, size_t stride) {
    byte depth = 0;
    // check bounds
    short length = octree_size(target_depth);
    if (pos.x >= length || pos.y >= length || pos.z >= length) {
        if (dbg_log_errors_get_octree_mut) {
            zox_logw("OOB [get_octree_mut] [%ix%ix%i] depth [%i] vlength [%i]", pos.x, pos.y, pos.z, target_depth, length);
        }
        return NULL;
    }
    while (node && depth < target_depth) {
        uintptr_t p = (uintptr_t) node;
        if (p == 0 || (p & 0x7) != 0) {
            return node;
        }
        // read the first-field pointer from the node
        void* kids_ptr = *(void**) node;
        if (!kids_ptr) {
            return node;
        }
        short div = octree_size(target_depth - depth - 1);
        if (div == 0) {
            break;
        }
        byte3 npos = { pos.x / div, pos.y / div, pos.z / div };
        byte3_modulus_byte(&pos, div);
        byte i = byte3_octree_array_index(npos);
        if (i >= 8) {
            if (dbg_log_errors_get_octree_mut) {
                zox_logw("[get_octree_mut] Invalid Index >= 8 [%i]\n  - pos [%ix%ix%i]\n    - npos [%ix%ix%i]\n    - div [%i]\n    - depth [%i]\n    - target_depth [%i]", i,
                         pos.x, pos.y, pos.z,
                         npos.x, npos.y, npos.z,
                         div, depth, target_depth);
            }
            return NULL;
        }
        node = (char*) kids_ptr + i * stride;
        depth++;
    }
    return node;
}*/

/*static inline void *get_octree_mut(
    void *node,
    byte target_depth,
    byte3 pos,
    size_t stride)
{
    if (!node) {
        return NULL;
    }
    short length = octree_size(target_depth);
    if (pos.x >= length || pos.y >= length || pos.z >= length) {
        if (dbg_log_errors_get_octree_mut) {
            zox_logw("OOB [get_octree_mut] [%ix%ix%i] depth [%i] vlength [%i]",
                     pos.x, pos.y, pos.z, target_depth, length);
        }
        return NULL;
    }
    byte depth = 0;
    while (depth < target_depth) {
        void *kids = *(void **)node;

        if (!kids) {
            return node;
        }
        byte shift = target_depth - depth - 1;
        byte mask = (1 << shift) - 1;
        byte3 npos = {
            pos.x >> shift,
            pos.y >> shift,
            pos.z >> shift
        };
        byte i = byte3_octree_array_index(npos);
        if (i >= 8) {
            if (dbg_log_errors_get_octree_mut) {
                zox_logw("[get_octree_mut] Invalid Index [%i]", i);
            }
            return NULL;
        }
        node = (char *)kids + i * stride;
        pos.x &= mask;
        pos.y &= mask;
        pos.z &= mask;
        depth++;
    }

    return node;
}*/

/*static inline void* open_octree_node(void* node, byte target_depth, byte3 pos, byte depth, size_t stride, size_t value_offset) {
    while (node && depth < target_depth) {
        uintptr_t p = (uintptr_t) node;
        // NOTE: Error checking nodes for corruption
        if (p == 0 || (p & 0x7) != 0) {
            return NULL;
        }
        void** kids_ptr_ref = (void**) node;
        void* kids_ptr = *kids_ptr_ref;
        // allocate children if missing
        if (!kids_ptr) {
            kids_ptr = malloc(8 * stride);
            if (!kids_ptr) {
                return NULL;
            }
            // optional: zero memory if required
            memset(kids_ptr, 0, 8 * stride);
            *kids_ptr_ref = kids_ptr;
            // --- New: set all children values ---
            byte parent_value = *(byte*)((char*) node + value_offset);
            for (byte j = 0; j < 8; j++) {
                void* child = (char*)(kids_ptr) + j * stride;
                *(byte*)((char*)child + value_offset) = parent_value;
            }
        }
        short div = octree_size(target_depth - depth - 1);
        if (div == 0) {
            break;
        }
        byte3 node_pos = { pos.x / div, pos.y / div, pos.z / div };
        byte3_modulus_byte(&pos, div);
        byte i = byte3_octree_array_index(node_pos);
        if (i >= 8) {
            return NULL;
        }
        node = (char*) kids_ptr + i * stride;
        depth++;
    }
    return node;
}*/

/*static inline void *open_octree_node(
    void *node,
    byte target_depth,
    byte3 pos,
    size_t stride,
    size_t value_offset)
{
    if (!node) {
        return NULL;
    }
    short length = octree_size(target_depth);
    if (pos.x >= length || pos.y >= length || pos.z >= length) {
        if (dbg_log_octree_errors) {
            zox_logw("OOB [open_octree_node] [%ix%ix%i] depth [%i] vlength [%i]",
                     pos.x, pos.y, pos.z, target_depth, length);
        }
        return NULL;
    }
    byte depth = 0;
    while (depth < target_depth) {
#ifdef zox_safety_checks
        uintptr_t p = (uintptr_t)node;
        if ((p & 0x7) != 0) {
            return NULL;
        }
#endif
        void **ptr = (void **)node;
        void *kids = *ptr;
        if (!kids) {
            kids = calloc(8, stride);
            if (!kids) {
                return NULL;
            }
            *ptr = kids;
            byte parent_value =
            *(byte *)((char *)node + value_offset);
            char *child = (char *)kids + value_offset;
            for (byte j = 0; j < 8; j++) {
                *(byte *)child = parent_value;
                child += stride;
            }
        }
        byte shift = target_depth - depth - 1;
        byte mask = (1 << shift) - 1;
        byte i =
            (pos.x >> shift) |
            ((pos.y >> shift) << 1) |
            ((pos.z >> shift) << 2);
#ifdef zox_safety_checks
        if (i >= 8) {
            return NULL;
        }
#endif
        node = (char *)kids + i * stride;
        pos.x &= mask;
        pos.y &= mask;
        pos.z &= mask;
        depth++;
    }
    return node;
}*/

// Core setter+collapse: same as set_octree_value but collapses upwards
/*static inline void* setreduce_octree_value(
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
    short div = octree_size(target_depth - depth - 1);
    if (div == 0) {
        return node;
    }
    byte3 node_pos = { pos.x / div, pos.y / div, pos.z / div };
    byte3_mod/*ulus_byte(&pos, div);
    byte i = byte3_octree_array_index(node_pos);
    if (i >= 8) {
        return node;
    }
    setreduce_octree_value((char*)kids + i * stride, target_depth, pos, value, depth + 1, stride, value_offset);
    // After child update, check if we can collapse this node
    collapse_octree_branch(node, stride, value_offset);
    return node;
// }*/
