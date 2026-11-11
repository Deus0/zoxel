// Octree Node Basics
//  - a node is { void* ptr; byte value; [byte type;] }, ptr first, value right after it
//  - type_offset: offsetof(T, type) for linked trees (VoxelNode), 0 for byte only trees
//      - linked nodes (type == node_type_instance) use ptr for an OctreeLink, not children
//      - every function that follows ptr goes through octree_children, so links are never walked into
//      - with type_offset 0 the compiler removes the type checks

#define octree_value_offset sizeof(void*)

static inline byte octree_is_linked(
    const void* node,
    size_t type_offset)
{
    return type_offset &&
        *(const byte*)((const char*) node + type_offset) == node_type_instance;
}

// NOTE: The only way to read a node's children, NULL for closed and linked nodes
static inline void* octree_children(
    const void* node,
    size_t type_offset)
{
    if (octree_is_linked(node, type_offset)) {
        return NULL;
    }
    return *(void* const*) node;
}

static inline void* octree_child(
    void* children,
    byte index,
    size_t stride)
{
    return (char*) children + index * stride;
}

// NOTE: frees the OctreeLink only, the linked entity is cleaned up separately
static inline byte octree_unlink(
    void* node,
    size_t type_offset)
{
    if (!octree_is_linked(node, type_offset)) {
        return 0;
    }
    zox_free(*(void**) node);
    *(void**) node = NULL;
    *(byte*)((char*) node + type_offset) = node_type_closed;
    return 1;
}

static inline void octree_create(
    void* node,
    size_t stride,
    byte value)
{
    memset(node, 0, stride);
    *(byte*)((char*) node + octree_value_offset) = value;
}

// allocates 8 children with value, node must be closed (not open or linked)
static inline byte octree_open(
    void* node,
    size_t stride,
    byte value)
{
    void** ptr = (void**) node;
    if (*ptr) {
        return 0;
    }
    void* kids = zox_malloc(stride * octree_length);
    if (!kids) {
        return 0;
    }
    *ptr = kids;
    for (byte i = 0; i < octree_length; i++) {
        octree_create(
            octree_child(kids, i, stride),
            stride,
            value);
    }
    return 1;
}

// frees all children and links below (and on) node, keeps node's value
static inline void octree_collapse(
    void* node,
    size_t stride,
    size_t type_offset)
{
    if (!node || octree_unlink(node, type_offset)) {
        return;
    }
    void* kids = octree_children(node, type_offset);
    if (!kids) {
        return;
    }
    for (byte i = 0; i < octree_length; i++) {
        octree_collapse(
            octree_child(kids, i, stride),
            stride,
            type_offset);
    }
    zox_free(kids);
    *(void**) node = NULL;
}

// collapses then resets everything but the value (type too)
static inline void octree_close(
    void* node,
    size_t stride,
    size_t type_offset)
{
    octree_collapse(node, stride, type_offset);
    byte value = *(byte*)((char*) node + octree_value_offset);
    octree_create(node, stride, value);
}

// deep copy, links are dropped: an entity lives in one place, the clone keeps the voxel value
static inline void octree_clone(
    void* dst,
    const void* src,
    size_t stride,
    size_t type_offset)
{
    memcpy(dst, src, stride);
    *(void**) dst = NULL;
    if (octree_is_linked(src, type_offset)) {
        *(byte*)((char*) dst + type_offset) = node_type_closed;
        return;
    }
    void* src_children = octree_children(src, type_offset);
    if (!src_children) {
        return;
    }
    if (!octree_open(dst, stride, 0)) {
        return;
    }
    void* dst_children = *(void**) dst;
    for (byte i = 0; i < octree_length; i++) {
        octree_clone(
            octree_child(dst_children, i, stride),
            octree_child(src_children, i, stride),
            stride,
            type_offset);
    }
}

// hands everything (children, link, value, type) to dst, src is reset to default_value
static inline void octree_move(
    void* dst,
    void* src,
    size_t stride,
    byte default_value)
{
    memcpy(dst, src, stride);
    octree_create(src, stride, default_value);
}

static inline byte is_on_edge_octree(byte depth, int3 position, byte direction) {
    if (depth >= 8) {
        return 0;
    }
    short length = octree_size(depth);
    position = move_position(position, direction);
    return !(position.x >= 0 && position.x < length && position.y >= 0 && position.y < length &&  position.z >= 0 && position.z < length);
}
