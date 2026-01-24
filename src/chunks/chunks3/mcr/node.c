#define zoxc_octree_fun1(T, base, default_value)\
\
zoxc_custom(T);\
\
zox_hookr(on_destroyed_##T, byte, (ecs* world, T* node), (world, node))\
\
static inline void create_lock_##T(T *node) { (void) node; }\
\
static inline void destroy_lock_##T(T *node) { (void) node; }\
\
static inline void write_lock_##T(const T *node) { (void) node; }\
\
static inline void write_unlock_##T(const T *node) { (void) node; }\
\
static inline void read_lock_##T(const T *node) { (void) node; }\
\
static inline void read_unlock_##T(const T *node) { (void) node; }\
\
static inline T* get_children_##T(const T *node) {\
    return (T*) node->ptr;\
}\
\
static inline byte is_opened_##T(const T *node) {\
    return node->ptr != NULL;\
}\
\
static inline byte is_closed_##T(const T *node) {\
    return node->ptr == NULL;\
}\
\
static inline byte has_children_##T(const T *node) {\
    /*return node->type == node_type_children;*/\
    return node->ptr && node->type == node_type_children;\
}\
\
void create_##T(T* node) {\
    memset(node, 0, sizeof(T)); \
    node->value = default_value;\
    zox_stats_nodes++; \
}\
\
void destroy_##T(ecs *world, T* node);\
\
void close_##T(ecs *world, T *node) {\
    if (!has_children_##T(node)) { \
        return; \
    } \
    T* kids = get_children_##T(node); \
    for (byte i = 0; i < octree_length; i++) { \
        destroy_##T(world, &kids[i]); \
    } \
    free(node->ptr); \
    base prev = node->value; \
    /* clears for any T types */ \
    memset(node, 0, sizeof(T)); \
    node->value = prev; \
    zox_stats_nodes--; \
}\
\
void destroy_##T(ecs *world, T* node) {\
    if (!is_closed_##T(node)) {\
        if (has_children_##T(node)) {\
            close_##T(world, node);\
        } else {\
            run_hook_on_destroyed_##T(world, node);\
        }\
    }\
    destroy_lock_##T(node);\
}\
\
ECS_DTOR(T, ptr, {\
    destroy_##T(local_world, ptr);\
})\
\
ECS_CTOR(T, ptr, {\
    create_##T(ptr);\
})\
\
void clone_##T(T*, const T*); \
\
ECS_COPY(T, dst, src, {\
    clone_##T(dst, src);\
})\
\
ECS_MOVE(T, dst, src, {\
    dst->ptr = src->ptr;\
    dst->value = src->value;\
    dst->type = src->type;\
    memset(src, 0, sizeof(T)); \
    src->value = default_value;\
})\
\
void dispose_system_##T(iter *it) {\
    zox_sys_world();\
    zox_sys_begin();\
    zox_sys_out(T);\
    for (int i = 0; i < it->count; i++) {\
        zox_sys_o(T, component);\
        \
        destroy_##T(world, component);\
    }\
} \
\
byte open_##T(T* node) { \
    void* ptr = (void*) malloc(sizeof(T) * octree_length);\
    if (ptr) { \
        node->ptr = ptr; \
        node->type = node_type_children; \
        T* kids = get_children_##T(node); \
        for (byte i = 0; i < octree_length; i++) { \
            create_##T(&kids[i]); \
        } \
    } \
    return ptr != NULL; \
} \
\
void clone_##T(\
    T* dst,\
    const T* src \
) {\
    dst->value = src->value;\
    dst->type = src->type;\
    if (src->type == node_type_instance) {\
        dst->ptr = src->ptr;\
    } else if (src->ptr) {\
        open_##T(dst);\
        T* kids_dst = get_children_##T(dst);\
        T* kids_src = get_children_##T(src);\
        for (byte i = 0; i < octree_length; i++) {\
            clone_##T(&kids_dst[i], &kids_src[i]);\
        }\
    } else {\
        dst->ptr = NULL;\
    }\
}

#define zoxd_node(T)\
    zoxd(T)\
    ecs_set_hooks(world, T, {\
        .ctor = ecs_ctor(T),\
        .move = ecs_move(T),\
        .copy = ecs_copy(T),\
        .dtor = ecs_dtor(T),\
    });\
    zox_observe_expr(dispose_system_##T, EcsOnRemove, "[out] "#T)

