// =======================================
// zox_node_getter.h
// Generic Octree Getter (ptr-first structs)
// =======================================

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

static inline const void *get_octree2(
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
            zox_logw("OOB [get_octree2] [%ix%ix%i] depth [%i] vlength [%i]",
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
            zox_loge("[get_octree2] Invalid Index [%i]", i);
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
}

static inline byte get_octree_value2(
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
}
