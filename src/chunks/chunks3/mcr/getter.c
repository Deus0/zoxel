// =======================================
// zox_node_getter.h
// Generic Octree Getter (ptr-first structs)
// =======================================

// Core: walk toward target depth, return deepest reachable node
static inline const void* find_octree_node(const void* node, byte target_depth, byte3 pos, byte depth, size_t stride) {
    while (node && depth < target_depth) {
        /* quick sanity: reject obviously bad pointers */
        uintptr_t p = (uintptr_t) node;
        if (p == 0 || (p & 0x7) != 0) {
            return node;
        }
        // read the first-field pointer from the node
        const void* kids_ptr = *(const void**) node;
        if (!kids_ptr) {
            return node;
        }
        const byte div = powers_of_two_byte[target_depth - depth - 1];
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

static inline void* find_octree_node_mut(void* node, byte target_depth, byte3 pos, byte depth, size_t stride) {
    while (node && depth < target_depth) {
        /* quick sanity: reject obviously bad pointers */
        uintptr_t p = (uintptr_t) node;
        if (p == 0 || (p & 0x7) != 0) {
            return node;
        }
        // read the first-field pointer from the node
        void* kids_ptr = *(void**) node;
        if (!kids_ptr) {
            return node;
        }
        byte div = powers_of_two_byte[target_depth - depth - 1];
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

static inline void* open_octree_node(void* node, byte target_depth, byte3 pos, byte depth, size_t stride, size_t value_offset) {
    while (node && depth < target_depth) {
        uintptr_t p = (uintptr_t) node;
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
        byte div = powers_of_two_byte[target_depth - depth - 1];
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

// Core: read node value at deepest reachable node
static inline byte read_octree_value(const void* node, byte target_depth, byte3 pos, byte depth, size_t stride, size_t value_offset) {
    node = find_octree_node(node, target_depth, pos, depth, stride);
    if (!node) {
        return 0;
    }

    /* quick sanity: reject obviously bad pointers */
    uintptr_t p = (uintptr_t) node;
    if (p == 0 || (p & 0x7) != 0) {
        return 0;
    }

    return *(byte*)((char*) node + value_offset);
}

// Macro wrapper: generates type-safe getters
#define create_node_getter(T) \
\
static inline const T* get_##T(const T* node, byte target_depth, byte3 pos, byte depth) { \
    return (T*)find_octree_node((const void*)node, target_depth, pos, depth, sizeof(T)); \
}\
\
static inline T* getm_##T(T* node, byte target_depth, byte3 pos, byte depth) { \
    return (T*) find_octree_node_mut((void*) node, target_depth, pos, depth, sizeof(T)); \
}\
\
static inline T* open_at_##T(T* node, byte target_depth, byte3 pos, byte depth) { \
    return (T*) open_octree_node((void*) node, target_depth, pos, depth, sizeof(T), offsetof(T, value)); \
}\
\
static inline byte get_value_##T(const T* node, byte target_depth, byte3 pos, byte depth) { \
    return read_octree_value((void*)node, target_depth, pos, depth, sizeof(T), offsetof(T, value)); \
}\
\
static inline byte getv_##T(const T* node, byte3 pos, byte target) { \
    return read_octree_value((void*)node, target, pos, 0, sizeof(T), offsetof(T, value)); \
}

// Example usage:
// create_node_getter(VoxelNode)
// create_node_getter(LightNode)
// VoxelNode* n = get_VoxelNode(root, target_depth, (byte3){x,y,z}, 0);
// byte v = get_value_VoxelNode(root, target_depth, (byte3){x,y,z}, 0);

