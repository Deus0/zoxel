// Macro wrapper: generates type-safe getters
#define create_octree_accessors(T) \
\
static inline const T* get_##T(const T* node, byte depth, byte3 position) { \
    return (T*) get_octree((const void*)node, depth, position, sizeof(T)); \
}\
\
static inline byte getv_##T(const T* node, byte depth, byte3 position) { \
    return get_octree_value((void*) node, depth, position,  sizeof(T), offsetof(T, value)); \
} \
\
static inline T* getm_##T(T* node, byte3 position, byte depth) { \
    return (T*) get_octree_mut((void*) node, depth, position, sizeof(T)); \
}\
\
static inline T* open_##T(T* node, byte depth, byte3 position) { \
    return (T*) open_octree_node((void*) node, depth, position, sizeof(T), offsetof(T, value)); \
}\
\
static inline T* open_one_##T(T* node) { \
    return (T*) open_octree_node((void*) node, 1, byte3_zero, sizeof(T), offsetof(T, value)); \
}

// Type-safe macro for root-first neighbor fetchers
#define create_node_neighbor(T) \
static inline const T* get_neighbor_##T(const T* root_node, const T** neighbors, byte dir, byte3 pos, byte depth) { \
    return (const T*)octree_get_adjacent_leaf( \
    (const void*) root_node, \
    (const void**) neighbors, \
    dir, \
    pos, \
    depth, \
    sizeof(T)\
    ); \
}

#define create_octree_optimizer(T, type) \
static inline void optimize_##T(T* node) { \
    optimize_octree_node( \
        (void*) node, \
        sizeof(T), \
        offsetof(T, value), \
        type \
    ); \
}

// NOTE: 27 neighbors
#define create_octree_get_nearby(T) \
static inline byte getv_nearby_##T(\
    const T** octrees, \
    spinlock** locks, \
    byte3 position,\
    byte depth, \
    sbyte3 offset,\
    byte oob_value) \
{ \
    return octree_getv_nearby(\
        (const void**) octrees, \
        locks, \
        position, \
        depth, \
        offset, \
        sizeof(T),\
        oob_value); \
}

#define create_octree_reducer(T, type) \
static inline byte reduce_##T(T* node) { \
    return reduce_octree_node( \
        (void*) node, \
        sizeof(T), \
        offsetof(T, value), \
        type \
    ); \
}

// create_octree_setter// Type-safe setters
#define create_octree_setters(T) \
static inline T* set_##T(T* node, byte depth, byte3 position, byte value) { \
    return (T*) set_octree_value2( \
        (void*) node, \
        depth, \
        position, \
        value, \
        1, \
        sizeof(T), \
        offsetof(T, value)); \
} \
\
static inline byte set_clean_##T(T* node, byte depth, byte3 position, byte value) { \
    return set_octree_value2( \
        (void*) node, \
        depth, \
        position, \
        value, \
        0, \
        sizeof(T), \
        offsetof(T, value)) != NULL; \
}

#define create_optimize_by_max(T) \
static inline void optimize_by_max_##T(T* node) { \
    reduce_to_max_octree_node((void*) node, sizeof(T), offsetof(T, value), 0); \
}

#define zoxc_octree_fun2(T, base)\
\
void clone_at_depth_##T(T* dst, const T* src, byte target_depth, byte depth) {\
    if (target_depth > 0 && depth == target_depth - 1) {\
        if (src->ptr) {\
            open_one_##T(dst);\
        }\
    }\
    if (depth == target_depth) {\
        dst->value = src->value;\
        /*dst->ptr = src->ptr;*/\
        /*dst->type = src->type;*/\
    } else {\
        if (src->ptr && dst->ptr) {\
            depth++;\
            T* kids_src = get_children_##T(src);\
            T* kids_dst = get_children_##T(dst);\
            for (byte i = 0; i < octree_length; i++) {\
                clone_at_depth_##T(&kids_dst[i], &kids_src[i], target_depth, depth);\
            }\
        }\
    }\
}\
\
/* maybe make below function use this if it isn't in the non root node */\
const T* get_adjacent_##T(const T** neighbors, const T* root, int3 position, byte depth, byte dir, byte *chunk_index) {\
    if (depth >= 8) {\
        return NULL;\
    }\
    position = move_position(position, dir); \
    short length = octree_size(depth); \
    if (position.x >= 0 && position.x < length && \
        position.y >= 0 && position.y < length && \
        position.z >= 0 && position.z < length) { \
        return get_##T(root, depth, int3_to_byte3(position)); \
        /*return gett_##T(node, position, depth);*/\
    } else {\
        /* special case for adjacent ptr, flips position and crosses to neighbor chunk */\
        *chunk_index = dir + 1;\
        const T* n = neighbors[dir]; \
        position = reverse_position(position, dir, length); \
        return get_##T(n, depth, int3_to_byte3(position)); \
        /*return gett_##T(n, position, depth); */\
    }\
}\
\
const T* get_adjacentn_##T(const T** neighbors, const T* vnode, int3 position, byte depth, byte direction) {\
    if (!vnode) { \
        return NULL; \
    } \
    short length = octree_size(depth); \
    position = move_position(position, direction); \
    if (position.x < 0 || position.x >= length || \
        position.y < 0 || position.y >= length || \
        position.z < 0 || position.z >= length) { \
        position = reverse_position(position, direction, length); \
        vnode = neighbors[direction];\
        if (!vnode) { \
            return NULL; \
        } \
    }\
    \
    return get_##T(vnode, depth, int3_to_byte3(position));\
}

#define zox_node_add_link(name)\
\
static inline byte is_linked_unlocked_##name(const name *node) {\
    return node->type == node_type_instance;\
}\
\
entity get_node_entity_unlocked_##name(const name *node) {\
    return is_linked_unlocked_##name(node) ? ((OctreeLink*) node->ptr)->value : 0;\
}\
\
static inline entity get_entity_##name(const name *node) {\
    const entity e = ((OctreeLink*) node->ptr)->value;\
    return e;\
}\
\
void link_node_##name(name* node, entity e) {\
    /*write_lock_##name(node);*/\
    /*if (node->type == node_type_closed) {*/\
    if (!node->ptr) {\
        node->type = node_type_instance;\
        node->ptr = zox_malloc(sizeof(OctreeLink));\
        *(OctreeLink*) node->ptr = (OctreeLink) { e };\
    }\
    /*write_unlock_##name(node);*/\
}\
\
byte dispose_node_link_##name(name *node) {\
    if (!node || !is_linked_##name(node)) {\
        return 0;\
    }\
    zox_free(node->ptr);\
    node->ptr = NULL;\
    node->type = node_type_closed;\
    return 1;\
}\
\
byte destroy_node_link_##name(ecs *world, name *node) {\
    if (!node || !is_linked_##name(node)) {\
        return 0;\
    }\
    /*write_lock_##name(node);*/\
    byte did_destroy = 0;\
    if (is_linked_##name(node)) {\
        entity e = get_node_entity_unlocked_##name(node);\
        \
        if (zox_valid(e)) {\
            zox_delete(e)\
            did_destroy = 1;\
        }\
        \
        zox_free(node->ptr);\
        node->ptr = NULL;\
        node->type = node_type_closed;\
    }\
    \
    /*write_unlock_##name(node);*/\
    return did_destroy;\
}\
\
entity get_node_entity_##name(const name *node) {\
    const entity e = is_linked_unlocked_##name(node) ? ((OctreeLink*) node->ptr)->value : 0;\
    return e;\
}
