static inline void octree_create(
    void* node,
    size_t stride,
    byte value)
{
    memset(node,0,stride);
    *(byte*)node = value;
}


static inline void octree_close(
    void* node,
    size_t stride)
{
    void** ptr = (void**)node;
    if (!*ptr) {
        return;
    }
    free(*ptr);
    byte value = *(byte*)node;
    memset(node, 0, stride);
    *(byte*)node = value;
}

static inline void octree_collapse(
    void* node,
    size_t stride)
{
    void** ptr = (void**)node;
    if (!*ptr) {
        return;
    }
    void* kids = *ptr;
    for (byte i = 0; i < octree_length; i++) {
        octree_collapse(
            (char*)kids + i * stride,
            stride);
    }
    free(kids);
    *ptr = NULL;
}

static inline byte octree_open(
    void* node,
    size_t stride,
    byte value)
{
    void** ptr = (void**)node;
    void* kids = malloc(stride * octree_length);
    if (!kids) {
        return 0;
    }
    *ptr = kids;
    for (byte i = 0; i < octree_length; i++) {
        octree_create(
            (char*)kids + i * stride,
            stride,
            value);
    }
    return 1;
}

static inline void octree_clone(
    void* dst,
    const void* src,
    size_t stride)
{
    *(byte*)dst = *(const byte*)src;
    void* src_children = *(void* const*)src;
    if (!src_children) {
        *(void**)dst = NULL;
        return;
    }
    octree_open(dst, stride, *(byte*)dst);
    void* dst_children = *(void**)dst;
    for (byte i = 0; i < octree_length; i++) {
        octree_clone(
            (char*)dst_children + i * stride,
            (char*)src_children + i * stride,
            stride);
    }
}

static inline void octree_move(
    void* dst,
    void* src,
    size_t stride,
    byte default_value)
{
    void** dst_ptr = dst;
    void** src_ptr = src;
    *dst_ptr = *src_ptr;
    *(byte*)dst = *(byte*)src;
    *src_ptr = NULL;
    *(byte*)src = default_value;
}


static inline byte is_on_edge_octree(byte depth, int3 position, byte direction) {
    if (depth >= 8) {
        return 0;
    }
    short length = octree_size(depth);
    position = move_position(position, direction);
    return !(position.x >= 0 && position.x < length && position.y >= 0 && position.y < length &&  position.z >= 0 && position.z < length);
}

static inline byte octree_is_open(const void* node)
{
    return *(void**) node != NULL;
}

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
        free(kids);
    }
    else if (type == node_type_instance) {
        destroy_link(world,node);
    }
    *(void**)node = NULL;
}