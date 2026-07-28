static inline void* open_octree_node(void* node, byte target_depth, byte3 pos, byte depth, size_t stride, size_t value_offset) {
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
}

static inline void *open_octree_node2(
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
            zox_logw("OOB [open_octree_node2] [%ix%ix%i] depth [%i] vlength [%i]",
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
}
