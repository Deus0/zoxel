// NOTE: Octree Components without Types
#define zoxc_octree(T, base, default_value)\
\
zoxc_custom(T);\
\
void create_##T(T* node) {\
    memset(node, 0, sizeof(T)); \
    node->value = default_value;\
    zox_stats_nodes++; \
}\
\
void close_##T(ecs *world, T *node) {\
    if (!node->ptr) { \
        return; \
    } \
    free(node->ptr); \
    base prev = node->value; \
    memset(node, 0, sizeof(T)); \
    node->value = prev; \
}\
\
void collapse_##T(T *node) {\
    if (!node->ptr) { \
        return; \
    } \
    T* kids = (T*) node->ptr; \
    for (byte i = 0; i < octree_length; i++) { \
        collapse_##T(&kids[i]); \
    } \
    free(node->ptr); \
    node->ptr = NULL; \
}\
\
byte zopen_##T(T* node) { \
    void* ptr = (void*) malloc(sizeof(T) * octree_length);\
    if (ptr) { \
        node->ptr = ptr; \
        T* kids = (T*) node->ptr; \
        for (byte i = 0; i < octree_length; i++) { \
            create_##T(&kids[i]); \
        } \
    } \
    return ptr != NULL; \
} \
\
void clone_##T(T* dst, const T* src) {\
    dst->value = src->value;\
    if (src->ptr) {\
        zopen_##T(dst);\
        T* kids_dst = (T*) dst->ptr;\
        T* kids_src =  (T*) src->ptr;\
        for (byte i = 0; i < octree_length; i++) {\
            clone_##T(&kids_dst[i], &kids_src[i]);\
        }\
    } else {\
        dst->ptr = NULL;\
    }\
} \
\
ECS_DTOR(T, ptr, {\
    collapse_##T(ptr);\
})\
\
ECS_CTOR(T, ptr, {\
    create_##T(ptr);\
})\
\
ECS_COPY(T, dst, src, {\
    clone_##T(dst, src);\
})\
\
ECS_MOVE(T, dst, src, {\
    dst->ptr = src->ptr;\
    dst->value = src->value;\
    memset(src, 0, sizeof(T)); \
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

