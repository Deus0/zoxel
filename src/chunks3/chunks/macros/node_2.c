#define zoxc_node(name, base, default_value)\
\
typedef struct name name;\
\
struct name {\
    base value;\
    byte type;\
    void* ptr;\
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
static inline byte is_opened_##name(const name *node) {\
    return node->ptr != NULL;\
}\
\
static inline byte is_closed_##name(const name *node) {\
    return node->ptr == NULL;\
}\
\
static inline name* get_children_##name(const name *node) {\
    name* children = node ? (name*) node->ptr : NULL;\
    return children;\
}\
\
static inline byte has_children_##name(const name *node) {\
    return node && node->type == node_type_children && node->ptr;\
}\
\
void create_##name(name* node) {\
    node->ptr = NULL;\
    node->value = default_value;\
    node->type = 0;\
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
    zox_stats_nodes--; \
    destroy_lock_##name(node);\
}\
\
ECS_CTOR(name, ptr, { \
    create_##name(ptr); \
}) \
\
ECS_DTOR(name, ptr, {\
    destroy_##name(local_world, ptr);\
})

#define zoxd_node(name)\
    zox_define_component(name)\
    ecs_set_hooks(world, name, {\
        .ctor = ecs_ctor(name),\
        .dtor = ecs_dtor(name),\
        .move = NULL,\
        .copy = NULL,\
    });
