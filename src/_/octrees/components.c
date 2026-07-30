#define zoxc_octree(T, base, default_value)\
\
zoxc_custom(T);\
\
void create_##T(T* node) {\
    octree_create(node, sizeof(T), default_value);\
}\
\
void close_##T(T* node) {\
    octree_close(node, sizeof(T));\
}\
\
void collapse_##T(T* node) {\
    octree_collapse(node, sizeof(T));\
}\
\
void clone_##T(T* dst, const T* src) {\
    octree_clone(dst, src, sizeof(T));\
}\
\
ECS_CTOR(T, ptr, {\
    create_##T(ptr);\
})\
\
ECS_COPY(T, dst, src, {\
    clone_##T(dst, src);\
})\
\
ECS_DTOR(T, ptr, {\
    collapse_##T(ptr);\
})


#define zoxc_octree_basic(T, base, default_value)\
zoxc_octree(T, base, default_value);\
\
ECS_MOVE(T, dst, src, {\
    octree_move(dst, src, sizeof(T), default_value);\
}) \

#define zoxc_octree_linked(T, base, default_value)\
zoxc_octree(T, base, default_value);\
\
/*zox_hookr(on_destroyed_##T, byte, (ecs* world, T* node), (world,node))*/\
\
static inline byte is_closed_##T(const T *node) {\
    return node->ptr == NULL;\
}\
\
static inline T* get_children_##T(const T* node) {\
    return (T*)node->ptr;\
}\
\
static inline byte has_children_##T(const T* node) {\
    return node->ptr && node->type == node_type_children;\
}\
\
static inline byte is_linked_##T(const T* node) {\
    return node->type == node_type_instance;\
}\
\
void destroy_##T(ecs* world, T* node) {\
    if (has_children_##T(node)) {\
        T* kids = get_children_##T(node);\
        for (byte i = 0; i < octree_length; i++) {\
            destroy_##T(world, &kids[i]);\
        }\
        free(node->ptr);\
        node->ptr = NULL;\
    } \
} \
\
ECS_MOVE(T, dst, src, {\
    dst->ptr = src->ptr;\
    dst->value = src->value;\
    dst->type = src->type;\
    memset(src, 0, sizeof(T));\
    src->value = default_value;\
})

#define zoxd_octree(T)\
    zoxd(T);\
    ecs_set_hooks(world, T, {\
        .ctor = ecs_ctor(T),\
        .move = ecs_move(T),\
        .copy = ecs_copy(T),\
        .dtor = ecs_dtor(T),\
    });
