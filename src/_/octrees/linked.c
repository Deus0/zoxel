static inline void octree_destroy_linked(
    ecs* world,
    void* node,
    size_t stride,
    size_t type_offset,
    byte (*destroy_link)(ecs*, void*))
{
    if (!node) {
        return;
    }
    byte type = *(byte*)((char*)node + type_offset);
    if (type == node_type_children) {
        void* kids = *(void**)node;
        for (byte i = 0; i < 8; i++) {
            octree_destroy_linked(
                world,
                (char*)kids + i * stride,
                stride,
                type_offset,
                destroy_link);
        }
        zox_free(kids);
    }
    else if (type == node_type_instance) {
        destroy_link(world,node);
    }
    *(void**)node = NULL;
}

static inline void octree_collapse_linked(
    void* node,
    size_t stride,
    size_t type_offset)
{
    if (!node) {
        return;
    }

    void** ptr = (void**)node;
    if (!*ptr) {
        return;
    }

    byte type = *(byte*)((char*)node + type_offset);

    if (type == node_type_children) {
        void* kids = *ptr;
        for (byte i = 0; i < octree_length; i++) {
            octree_collapse_linked(
                (char*)kids + i * stride,
                stride,
                type_offset);
        }
        zox_free(kids);
    }

    *ptr = NULL;
}


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
ECS_MOVE(T, dst, src, {\
    dst->ptr = src->ptr;\
    dst->value = src->value;\
    dst->type = src->type;\
    memset(src, 0, sizeof(T));\
    src->value = default_value;\
}) \
\
ECS_DTOR(T, ptr, { \
    octree_collapse_linked( \
        ptr, \
        sizeof(T), \
        offsetof(T,type)); \
})
