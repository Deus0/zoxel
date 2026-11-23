// Type-safe wrappers, T must be declared with zoxc_octree_* (or zox_octree_functions)
//  - all pass T's type offset, so linked nodes are never walked into

#define create_octree_accessors(T) \
\
static inline const T* get_##T(const T* node, byte depth, byte3 position) { \
    return (const T*) get_octree((const void*) node, depth, position, sizeof(T), octree_type_offset_##T); \
}\
\
static inline byte getv_##T(const T* node, byte depth, byte3 position) { \
    return get_octree_value((const void*) node, depth, position, sizeof(T), offsetof(T, value), octree_type_offset_##T); \
} \
\
static inline T* getm_##T(T* node, byte depth, byte3 position) { \
    return (T*) get_octree_mut((void*) node, depth, position, sizeof(T), octree_type_offset_##T); \
}\
\
static inline T* open_##T(T* node, byte depth, byte3 position) { \
    return (T*) open_octree_node((void*) node, depth, position, sizeof(T), offsetof(T, value), octree_type_offset_##T); \
}\
\
static inline T* open_one_##T(T* node) { \
    return (T*) open_octree_node((void*) node, 1, byte3_zero, sizeof(T), offsetof(T, value), octree_type_offset_##T); \
}

// Root-first neighbor fetchers
#define create_node_neighbor(T) \
static inline const T* get_neighbor_##T(const T* root_node, const T** neighbors, byte dir, byte3 pos, byte depth) { \
    return (const T*) octree_get_adjacent_leaf( \
        (const void*) root_node, \
        (const void**) neighbors, \
        dir, \
        pos, \
        depth, \
        sizeof(T), \
        octree_type_offset_##T); \
}

// skip_air: air never wins the mode (voxels), 0 for plain values (lights, sides)
#define create_octree_optimizer(T, skip_air) \
static inline void optimize_##T(T* node) { \
    optimize_octree_node( \
        (void*) node, \
        sizeof(T), \
        offsetof(T, value), \
        octree_type_offset_##T, \
        skip_air); \
} \
\
static inline byte optimize_reduce_##T(T* node) { \
    return optimize_reduce_octree_node( \
        (void*) node, \
        sizeof(T), \
        offsetof(T, value), \
        octree_type_offset_##T, \
        skip_air); \
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
        octree_type_offset_##T,\
        oob_value); \
}

// the max of the leaves under the voxel at a lod's depth (lights read by coarse lod meshes)
#define create_octree_get_nearby_max(T) \
static inline byte getv_nearby_max_##T(const T** octrees, spinlock** locks, byte3 position, byte depth, sbyte3 offset, byte oob_value) { \
    return octree_getv_nearby_ex((const void**) octrees, locks, position, depth, offset, sizeof(T), octree_type_offset_##T, oob_value, 1); \
}

#define create_octree_reducer(T) \
static inline byte reduce_##T(T* node) { \
    return reduce_octree_node( \
        (void*) node, \
        sizeof(T), \
        offsetof(T, value), \
        octree_type_offset_##T); \
}

#define create_octree_setters(T) \
static inline T* set_##T(T* node, byte depth, byte3 position, byte value) { \
    return (T*) set_octree_value2( \
        (void*) node, \
        depth, \
        position, \
        value, \
        1, \
        sizeof(T), \
        offsetof(T, value), \
        octree_type_offset_##T); \
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
        offsetof(T, value), \
        octree_type_offset_##T) != NULL; \
}

#define create_optimize_by_max(T) \
static inline void optimize_by_max_##T(T* node) { \
    reduce_to_max_octree_node((void*) node, sizeof(T), offsetof(T, value), octree_type_offset_##T); \
}

// Queries
#define create_octree_queries(T) \
static inline byte get_max_value_##T(const T* node) { \
    return get_octree_max_value((const void*) node, sizeof(T), offsetof(T, value), octree_type_offset_##T); \
} \
\
static inline byte is_in_##T(const T* node, byte value) { \
    return is_in_octree((const void*) node, sizeof(T), offsetof(T, value), octree_type_offset_##T, value); \
} \
\
static inline byte3 get_bounds_##T(const T* node, byte depth) { \
    return get_octree_bounds((const void*) node, depth, sizeof(T), offsetof(T, value), octree_type_offset_##T); \
}

#define zoxc_octree_fun2(T, base)\
\
void clone_at_depth_##T(T* dst, const T* src, byte target_depth, byte depth) {\
    if (target_depth > 0 && depth == target_depth - 1) {\
        if (has_children_##T(src)) {\
            open_one_##T(dst);\
        }\
    }\
    if (depth == target_depth) {\
        dst->value = src->value;\
        /*dst->ptr = src->ptr;*/\
        /*dst->type = src->type;*/\
    } else {\
        if (has_children_##T(src) && has_children_##T(dst)) {\
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
const T* get_adjacentn_##T(\
    const T** neighbors, \
    const T* vnode, \
    int3 position, \
    byte depth,\
    byte direction) \
{\
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

// a linked tree's (VoxelNode's) links: a node holding an entity (a block vox) in place of children
#define zox_node_add_link(name)\
\
static inline entity get_entity_##name(const name *node) {\
    return ((OctreeLink*) node->ptr)->value;\
}\
\
entity get_node_entity_##name(const name *node) {\
    return is_linked_##name(node) ? get_entity_##name(node) : 0;\
}\
\
void link_node_##name(name* node, entity e) {\
    if (!node->ptr) {\
        node->type = node_type_instance;\
        node->ptr = zox_malloc(sizeof(OctreeLink));\
        *(OctreeLink*) node->ptr = (OctreeLink) { e };\
    }\
}\
\
byte dispose_node_link_##name(name *node) {\
    if (!node || !is_linked_##name(node)) {\
        return 0;\
    }\
    return octree_unlink(node, octree_type_offset_##name);\
}\
\
byte destroy_node_link_##name(ecs *world, name *node) {\
    if (!node || !is_linked_##name(node)) {\
        return 0;\
    }\
    const entity e = get_entity_##name(node);\
    const byte did_destroy = zox_valid(e);\
    if (did_destroy) {\
        zox_delete(e)\
    }\
    octree_unlink(node, octree_type_offset_##name);\
    return did_destroy;\
}
