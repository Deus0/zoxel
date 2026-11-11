// Octree Types
//  - every octree type uses the same functions, only its type offset differs
//      - zoxc_octree_basic: byte only nodes { ptr, value }, type offset 0
//      - zoxc_octree_linked: nodes { ptr, value, type }, ptr is an OctreeLink when type is node_type_instance
//  - zox_octree_functions alone gives the functions without a component (tests)

#define zox_octree_functions(T, default_value, type_offset_value)\
static const size_t octree_type_offset_##T = type_offset_value;\
\
static inline void create_##T(T* node) {\
    octree_create(node, sizeof(T), default_value);\
}\
\
static inline void close_##T(T* node) {\
    octree_close(node, sizeof(T), octree_type_offset_##T);\
}\
\
static inline void collapse_##T(T* node) {\
    octree_collapse(node, sizeof(T), octree_type_offset_##T);\
}\
\
static inline void clone_##T(T* dst, const T* src) {\
    octree_clone(dst, src, sizeof(T), octree_type_offset_##T);\
}\
\
static inline void move_##T(T* dst, T* src) {\
    octree_move(dst, src, sizeof(T), default_value);\
}\
\
static inline T* get_children_##T(const T* node) {\
    return (T*) octree_children(node, octree_type_offset_##T);\
}\
\
static inline byte has_children_##T(const T* node) {\
    return get_children_##T(node) != NULL;\
}\
\
static inline byte is_linked_##T(const T* node) {\
    return octree_is_linked(node, octree_type_offset_##T);\
}\
\
static inline byte is_closed_##T(const T* node) {\
    return node->ptr == NULL;\
}

// NOTE: flecs copy/move assign into a constructed dst, which may hold a tree, so free it first
#define zoxc_octree(T, base, default_value, type_offset_value)\
zoxc_custom(T);\
zox_octree_functions(T, default_value, type_offset_value)\
\
ECS_CTOR(T, ptr, {\
    create_##T(ptr);\
})\
\
ECS_COPY(T, dst, src, {\
    collapse_##T(dst);\
    clone_##T(dst, src);\
})\
\
ECS_MOVE(T, dst, src, {\
    collapse_##T(dst);\
    move_##T(dst, src);\
})\
\
ECS_DTOR(T, ptr, {\
    collapse_##T(ptr);\
})

#define zoxc_octree_basic(T, base, default_value)\
    zoxc_octree(T, base, default_value, 0)

#define zoxc_octree_linked(T, base, default_value)\
    zoxc_octree(T, base, default_value, offsetof(T, type))

#define zoxd_octree(T)\
    zoxd(T);\
    ecs_set_hooks(world, T, {\
        .ctor = ecs_ctor(T),\
        .move = ecs_move(T),\
        .copy = ecs_copy(T),\
        .dtor = ecs_dtor(T),\
    });
