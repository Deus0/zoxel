byte dbg_log_errors_get_octree_mut = 1;

// TODO: Just make all these start at 0 depth
static inline void* get_octree_mut(void* node, byte target_depth, byte3 pos, size_t stride) {
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
}
static inline void *get_octree_mut2(
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
            zox_logw("OOB [get_octree_mut2] [%ix%ix%i] depth [%i] vlength [%i]",
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
                zox_logw("[get_octree_mut2] Invalid Index [%i]", i);
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
}
