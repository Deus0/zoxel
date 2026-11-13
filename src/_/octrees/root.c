// Root Traversal Functions

typedef enum {
    OCTREE_WALK_STOP,   // stop at deepest existing node
    OCTREE_WALK_OPEN    // allocate missing children
} octree_walk_mode;

typedef void (*octree_open_hook) (
    void *parent,
    void *children,
    size_t stride,
    void *user);

// walks from node to target_depth at position
//  - STOP: returns the deepest existing node (a closed or linked node above target_depth)
//  - OPEN: allocates missing children down to target_depth, NULL if a link is in the way
static inline void *walk_octree(
    void *node,
    byte target_depth,
    byte3 position,
    size_t stride,
    size_t type_offset,
    octree_walk_mode mode,
    octree_open_hook hook,
    void *hook_user)
{
    if (!node) {
        return NULL;
    }
    ushort length = octree_size(target_depth);
    if (position.x >= length ||
        position.y >= length ||
        position.z >= length) {
        return NULL;
    }
    for (byte depth = 0; depth < target_depth; depth++) {
#ifdef zox_safety_checks
        if (((uintptr_t) node & 7) != 0) {
            return NULL;
        }
#endif
        void *children = octree_children(node, type_offset);
        if (!children) {
            if (mode == OCTREE_WALK_STOP) {
                return node;
            }
            // NOTE: can't open through a link, unlink it first
            if (octree_is_linked(node, type_offset)) {
                return NULL;
            }
            children = zox_malloc(stride * octree_length);
            if (!children) {
                return NULL;
            }
            memset(children, 0, stride * octree_length);
            *(void **) node = children;
            if (hook) {
                hook(node, children, stride, hook_user);
            }
        }
        byte shift = target_depth - depth - 1;
        byte index = byte3_octree_array_index((byte3){
            position.x >> shift,
            position.y >> shift,
            position.z >> shift
        });
        node = octree_child(children, index, stride);
        if (shift) {
            byte mask = (1u << shift) - 1u;
            position.x &= mask;
            position.y &= mask;
            position.z &= mask;
        }
    }
    return node;
}

static inline const void *get_octree(
    const void *node,
    byte target_depth,
    byte3 position,
    size_t stride,
    size_t type_offset)
{
    return walk_octree(
        (void *)node,
        target_depth,
        position,
        stride,
        type_offset,
        OCTREE_WALK_STOP,
        NULL,
        NULL);
}

static inline void* get_octree_mut(
    void *node,
    byte target_depth,
    byte3 position,
    size_t stride,
    size_t type_offset)
{
    return walk_octree(
        node,
        target_depth,
        position,
        stride,
        type_offset,
        OCTREE_WALK_STOP,
        NULL,
        NULL);
}

static inline void inherit_parent_value(
    void *parent,
    void *children,
    size_t stride,
    void *user)
{
    size_t value_offset = (size_t)user;
    byte value = *(byte *)((char *)parent + value_offset);
    for (byte i = 0; i < 8; i++) {
        *(byte *)((char *)children + i *stride + value_offset) = value;
    }
}

static inline void *open_octree_node(
    void *node,
    byte target_depth,
    byte3 position,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    return walk_octree(
        node,
        target_depth,
        position,
        stride,
        type_offset,
        OCTREE_WALK_OPEN,
        inherit_parent_value,
        (void *)value_offset);
}

static inline void *set_octree_value2(
    void *node,
    byte target_depth,
    byte3 position,
    byte value,
    byte inherit_parent_value_flag,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    node = walk_octree(
        node,
        target_depth,
        position,
        stride,
        type_offset,
        OCTREE_WALK_OPEN,
        inherit_parent_value_flag ? inherit_parent_value : NULL,
        (void *)value_offset);
    if (node) {
        *(byte *)((char *)node + value_offset) = value;
    }
    return node;
}

static inline byte get_octree_value(
    const void *node,
    byte target_depth,
    byte3 position,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    node = walk_octree(
        (void *)node,
        target_depth,
        position,
        stride,
        type_offset,
        OCTREE_WALK_STOP,
        NULL,
        NULL);
    if (!node) {
        return 0;
    }
    return *(const byte *)((const char *)node + value_offset);
}

static inline byte3 byte3_offset_wrap(
    byte3 position,
    byte depth,
    sbyte3 offset,
    sbyte3 *chunk_offset)
{
    ushort size = octree_size(depth);
    sbyte3 result = {
        position.x + offset.x,
        position.y + offset.y,
        position.z + offset.z
    };
    chunk_offset->x = 0;
    chunk_offset->y = 0;
    chunk_offset->z = 0;
    if (result.x < 0) {
        chunk_offset->x = -1;
        result.x += size;
    } else if (result.x >= size) {
        chunk_offset->x = 1;
        result.x -= size;
    }
    if (result.y < 0) {
        chunk_offset->y = -1;
        result.y += size;
    } else if (result.y >= size) {
        chunk_offset->y = 1;
        result.y -= size;
    }
    if (result.z < 0) {
        chunk_offset->z = -1;
        result.z += size;
    } else if (result.z >= size) {
        chunk_offset->z = 1;
        result.z -= size;
    }
    return (byte3) {
        result.x,
        result.y,
        result.z
    };
}

// NOTE: Gets voxel from neighbouring chunk if position crosses boundary

// the max value of a node's leaves, an open node's own value is stale (set when it split), only its leaves count
static inline byte octree_max_leaf_value(const void* node, size_t stride, size_t type_offset) {
    const void* children = octree_children(node, type_offset);
    if (!children) {
        return *(const byte*)((const char*) node + octree_value_offset);
    }
    byte max = 0;
    for (byte i = 0; i < octree_length; i++) {
        const byte value = octree_max_leaf_value(octree_child((void*) children, i, stride), stride, type_offset);
        max = value > max ? value : max;
    }
    return max;
}

// NOTE: Gets voxel from neighbouring chunk if position crosses boundary
//  - leaf_max: a node above the leaves (a coarse lod reading a detailed tree) gives its leaves' max, not its own value
static inline byte octree_getv_nearby_ex(
    const void** octrees,
    spinlock** locks,
    byte3 position,
    byte depth,
    sbyte3 offset,
    size_t stride,
    size_t type_offset,
    byte oob_value,
    byte leaf_max)
{
    if (!octrees || depth >= 7) {
        return oob_value;
    }
    sbyte3 chunk_offset;
    byte3 local_position = byte3_offset_wrap(
        position,
        depth,
        offset,
        &chunk_offset
    );
    byte index = OCTREE_INDEX(
        chunk_offset.x,
        chunk_offset.y,
        chunk_offset.z
    );
    const void* root = octrees[index];
    spinlock* lock = locks[index];
    if (!root) {
        return oob_value;
    }
    if (!lock) {
        zox_loge("LOCK INVALID [%i]", index);
        return oob_value;
    }
    spin_lock(lock);
    const void* octree = get_octree(
        root,
        depth,
        local_position,
        stride,
        type_offset
    );
    const byte value = !octree ? oob_value :
        leaf_max ? octree_max_leaf_value(octree, stride, type_offset) :
        *(const byte*)((const char*) octree + octree_value_offset);
    spin_unlock(lock);
    return value;
}

static inline byte octree_getv_nearby(
    const void** octrees,
    spinlock** locks,
    byte3 position,
    byte depth,
    sbyte3 offset,
    size_t stride,
    size_t type_offset,
    byte oob_value)
{
    return octree_getv_nearby_ex(octrees, locks, position, depth, offset, stride, type_offset, oob_value, 0);
}

// dir ordering:
// 0 -X
// 1 +X
// 2 -Y
// 3 +Y
// 4 -Z
// 5 +Z
static inline const void* octree_get_adjacent_leaf(
    const void* root_node,
    const void** neighbors,
    byte dir,
    byte3 pos,
    byte depth,
    size_t stride,
    size_t type_offset)
{
    if (!root_node || dir > 5) {
        return NULL;
    }
    uint size = 1u << depth;
    uint max = size - 1;
    byte axis = dir >> 1;
    byte positive = dir & 1;
    byte coordinate =
        axis == 0 ? pos.x :
        axis == 1 ? pos.y :
                    pos.z;
    byte boundary = positive
        ? coordinate == max
        : coordinate == 0;
    // Crossed into neighbour chunk
    if (boundary) {
        if (!neighbors) {
            return NULL;
        }
        const void* neighbour = neighbors[dir];
        if (!neighbour) {
            return NULL;
        }
        byte3 neighbour_pos = pos;
        if (axis == 0) {
            neighbour_pos.x = positive ? 0 : max;
        } else if (axis == 1) {
            neighbour_pos.y = positive ? 0 : max;
        } else {
            neighbour_pos.z = positive ? 0 : max;
        }
        return get_octree(
            neighbour,
            depth,
            neighbour_pos,
            stride,
            type_offset
        );
    }
    // Same chunk
    if (axis == 0) {
        pos.x += positive ? 1 : -1;
    } else if (axis == 1) {
        pos.y += positive ? 1 : -1;
    } else {
        pos.z += positive ? 1 : -1;
    }
    return get_octree(
        root_node,
        depth,
        pos,
        stride,
        type_offset
    );
}
