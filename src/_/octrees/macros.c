

// Macro wrapper: generates type-safe getters
#define create_node_getter(T) \
\
static inline const T* get_##T(const T* node, byte depth, byte3 position) { \
    return (T*) get_octree2((const void*)node, depth, position, sizeof(T)); \
}\
\
static inline byte getv_##T(const T* node, byte depth, byte3 position) { \
    return get_octree_value2((void*) node, depth, position,  sizeof(T), offsetof(T, value)); \
} \
\
static inline T* getm_##T(T* node, byte3 position, byte depth) { \
    return (T*) get_octree_mut2((void*) node, depth, position, sizeof(T)); \
}\
\
static inline T* open_##T(T* node, byte depth, byte3 position) { \
    return (T*) open_octree_node2((void*) node, depth, position, sizeof(T), offsetof(T, value)); \
}\
\
static inline T* open_one_##T(T* node) { \
    return (T*) open_octree_node2((void*) node, 1, byte3_zero, sizeof(T), offsetof(T, value)); \
}

// Example usage:
// create_node_getter(VoxelNode)
// create_node_getter(LightNode)
// VoxelNode* n = get_VoxelNode(root, target_depth, (byte3){x,y,z});
// byte v = getv_VoxelNode(root, target_depth, (byte3){x,y,z});

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


// Macro wrapper
#define create_octree_optimizer(T) \
static inline void optimize_##T(T* node) { \
    optimize_octree_node((void*) node, sizeof(T), offsetof(T, value), 0); \
}

#define create_octree_optimizer_linked(T) \
static inline void optimize_##T(T* node) { \
    optimize_octree_node((void*) node, sizeof(T), offsetof(T, value), offsetof(T, type)); \
}

// Usage:
// create_node_optimizer(LightNode)
// optimize_LightNode(lroot);


// Macro wrapper: type-safe setter
#define new_octree_function_set(T) \
static inline T* set_##T(T* node, byte depth, byte3 position, byte value) { \
    return (T*) set_octree_value2((void*) node, depth, position, value, 1, sizeof(T), offsetof(T, value)); \
}

// Example usage:
// create_node_setter(VoxelNode)
// create_node_setter(LightNode)
// set_VoxelNode(vroot, 5, (byte3){x,y,z}, 1, 0);
// set_LightNode(lroot, 5, (byte3){x,y,z}, 255, 0);

// Macro wrapper: type-safe setter
#define new_octree_function_set_clean(T) \
static inline byte set_clean_##T(T* node, byte depth, byte3 position, byte value) { \
    return set_octree_value2((void*) node, depth, position, value, 0, sizeof(T), offsetof(T, value)) != NULL; \
}

// Example usage:
// new_octree_function_set_clean(VoxelNode)
// set_clean_VoxelNode(vroot, 5, (byte3){x,y,z}, 1, 0);



// Macro wrapper: type-safe setreduce
#define create_node_setreduce(T) \
static inline T* setreduce_##T##_ex(T* node, byte target_depth, byte3 pos, byte value, byte depth) { \
    return (T*)setreduce_octree_value((void*)node, target_depth, pos, value, depth, sizeof(T), offsetof(T, value)); \
}

// Example usage:
// create_node_setreduce(LightNode)
// setreduce_LightNode(lroot, 5, (byte3){x,y,z}, 0, 0);


// NOTE: 27 neighbors
#define create_octree_get_nearby(T) \
static inline const T* get_nearby_##T(const T** octrees, byte3 position, byte depth, sbyte3 offset) { \
    return (const T*) octree_get_nearby((const void**) octrees, position, depth, offset, sizeof(T)); \
} \
static inline byte getv_nearby_##T(const T** octrees, byte3 position, byte depth, sbyte3 offset) { \
    const T* thing = (const T*) octree_get_nearby((const void**) octrees, position, depth, offset, sizeof(T)); \
    return thing ? thing->value : 0; \
}

// TODO: Function to grab 27 neighbors from chunk + neighbors
// TODO: remove depth from get_octree and just use target depth


// Macro wrapper: type-safe reducer
#define create_octree_reducer(T) \
static inline byte reduce_##T(T* node) { \
    return reduce_octree_node((void*) node, sizeof(T), offsetof(T, value), 0); \
}

#define create_octree_reducer_linked(T) \
static inline byte reduce_##T(T* node) { \
    return reduce_octree_node((void*) node, sizeof(T), offsetof(T, value), offsetof(T, type)); \
}

// Usage:
// create_node_reducer(LightNode)
// reduce_LightNode(lroot);


// Macro wrapper
#define create_optimize_by_max(T) \
static inline void optimize_by_max_##T(T* node) { \
    reduce_to_max_octree_node((void*) node, sizeof(T), offsetof(T, value), 0); \
}

// Usage:
// create_optimize_by_max(LightNode);
// optimize_by_max_LightNode(lroot);
