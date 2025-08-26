#define octree_value_struct_offset sizeof(void*)

#define zoxc_node(name, base, default_value)\
\
typedef struct name name;\
\
struct name {\
    void* ptr;\
    base value;\
    byte type;\
    zox_lock lock;\
};\
zoxc_custom(name);\
\
zox_hookr(on_destroyed_##name, byte, (ecs* world, name* node), (world, node))\
\
static inline void create_lock_##name(name *node) {\
    if (nodes_w_safety_locks || nodes_r_safety_locks) {\
        zox_lock_init(&node->lock);\
    }\
}\
\
static inline void destroy_lock_##name(name *node) {\
    if (nodes_w_safety_locks || nodes_r_safety_locks) {\
        zox_lock_destroy(&node->lock);\
    }\
}\
\
static inline void write_lock_##name(const name *node) {\
    if (nodes_w_safety_locks) {\
        zox_lock_write(&node->lock);\
    }\
}\
\
static inline void write_unlock_##name(const name *node) {\
    if (nodes_w_safety_locks) {\
        zox_unlock_write(&node->lock);\
    }\
}\
\
static inline void read_lock_##name(const name *node) {\
    if (nodes_r_safety_locks) {\
        zox_lock_read(&node->lock);\
    }\
}\
\
static inline void read_unlock_##name(const name *node) {\
    if (nodes_r_safety_locks) {\
        zox_unlock_read(&node->lock);\
    }\
}\
\
static inline name* get_children_##name(const name *node) {\
    return (name*) node->ptr;\
}\
\
static inline byte is_opened_##name(const name *node) {\
    return node->ptr != NULL;\
}\
\
static inline byte is_closed_##name(const name *node) {\
    return node->ptr == NULL;\
}\
\
static inline byte has_children_##name(const name *node) {\
    return node->type == node_type_children;\
}\
\
void create_##name(name* node) {\
    node->ptr = NULL;\
    node->type = 0;\
    node->value = default_value;\
    create_lock_##name(node);\
    zox_stats_nodes++; \
}\
\
void destroy_##name(ecs *world, name* node);\
\
void close_##name(ecs *world, name *node) {\
    if (!has_children_##name(node)) { \
        return; \
    } \
    name* kids = get_children_##name(node); \
    for (byte i = 0; i < octree_length; i++) { \
        destroy_##name(world, &kids[i]); \
    } \
    node->type = node_type_closed; \
    free(node->ptr); \
    node->ptr = NULL; \
    zox_stats_nodes--; \
}\
\
void destroy_##name(ecs *world, name* node) {\
    if (!is_closed_##name(node)) {\
        if (has_children_##name(node)) {\
            close_##name(world, node);\
        } else {\
            run_hook_on_destroyed_##name(world, node);\
        }\
    }\
    destroy_lock_##name(node);\
}\
\
ECS_DTOR(name, ptr, {\
    destroy_##name(local_world, ptr);\
})\
\
ECS_CTOR(name, ptr, {\
    create_##name(ptr);\
})\
\
void clone_##name(name*, const name*); \
\
ECS_COPY(name, dst, src, {\
    clone_##name(dst, src);\
})\
\
ECS_MOVE(name, dst, src, {\
    dst->ptr = src->ptr;\
    dst->value = src->value;\
    dst->type = src->type;\
    src->ptr = NULL;\
    src->value = default_value;\
    src->type = 0;\
})\
\
void dispose_system_##name(iter *it) {\
    zox_sys_world()\
    zox_sys_begin()\
    zox_sys_out(name)\
    for (int i = 0; i < it->count; i++) {\
        zox_sys_o(name, component)\
        destroy_##name(world, component);\
    }\
}

#define zoxd_node(name)\
    zoxd(name)\
    ecs_set_hooks(world, name, {\
        .ctor = ecs_ctor(name),\
        .move = ecs_move(name),\
        .copy = ecs_copy(name),\
        .dtor = ecs_dtor(name),\
    });\
    zox_observe_expr(dispose_system_##name, EcsOnRemove, "[out] "#name)

