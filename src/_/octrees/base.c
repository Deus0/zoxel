static inline void octree_create(
    void* node,
    size_t stride,
    byte value)
{
    memset(node,0,stride);
    // *(byte*)node = value;
    *(byte*)((char*)node + sizeof(void*)) = value;
}


static inline void octree_close(
    void* node,
    size_t stride)
{
    void** ptr = (void**)node;
    if (!*ptr) {
        return;
    }
    zox_free(*ptr);
    // NOTE: value is after the children pointer
    byte value = *(byte*)((char*)node + sizeof(void*));
    memset(node, 0, stride);
    *(byte*)((char*)node + sizeof(void*)) = value;
}

static inline byte octree_open(
    void* node,
    size_t stride,
    byte value)
{
    void** ptr = (void**)node;
    void* kids = zox_malloc(stride * octree_length);
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
    // NOTE: Copies value + type, which come after the children pointer
    memcpy(dst, src, stride);
    void* src_children = *(void* const*)src;
    if (!src_children) {
        return;
    }
    *(void**)dst = NULL;
    octree_open(dst, stride, *(const byte*)((const char*)src + sizeof(void*)));
    void* dst_children = *(void**)dst;
    if (!dst_children) {
        return;
    }
    for (byte i = 0; i < octree_length; i++) {
        octree_clone(
            (char*)dst_children + i * stride,
            (char*)src_children + i * stride,
            stride);
    }
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

static inline void octree_collapse(
    void* node,
    size_t stride)
{
    void** ptr = (void**) node;
    if (!*ptr) {
        return;
    }
    void* kids = *ptr;
    for (byte i = 0; i < octree_length; i++) {
        octree_collapse(
            (char*)kids + i * stride,
            stride);
    }
    zox_free(kids);
    *ptr = NULL;
}

static inline void octree_move(
    void* dst,
    void* src,
    size_t stride,
    byte default_value)
{
    *(void**) dst = *(void**) src;
    *(byte*)((char*) dst + sizeof(void*)) = *(byte*)((char*) src + sizeof(void*));
    *(void**) src = NULL;
    *(byte*)((char*) src + sizeof(void*)) = default_value;
    /*void** dst_ptr = dst;
    void** src_ptr = src;
    *dst_ptr = *src_ptr;
    *(byte*)dst = *(byte*)src;
    *src_ptr = NULL;
    *(byte*)src = default_value;*/
}

static inline void free_octree(void* node, size_t stride) {
    if (!node) {
        return;
    }
    void** ptr = (void**) node;
    void* kids = *ptr;
    if (!kids) {
        return;
    }
    for (byte i = 0; i < 8; i++) {
        free_octree(
            (char*) kids + i * stride,
            stride
        );
    }
    zox_free(kids);
    *ptr = NULL;
}