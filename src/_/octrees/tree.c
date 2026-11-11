// Tree Traversal Functions
//  - linked nodes count as leaves with their value, they are never collapsed or walked into

static inline byte get_octree_max_value(
    const void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    if (!node) {
        return 0;
    }
    byte max = *(const byte*)((const char*) node + value_offset);
    void* children = octree_children(node, type_offset);
    if (!children) {
        return max;
    }
    for (byte i = 0; i < octree_length; i++) {
        byte child_max = get_octree_max_value(
            octree_child(children, i, stride),
            stride,
            value_offset,
            type_offset);
        if (child_max > max) {
            max = child_max;
        }
    }
    return max;
}

// collapses node if its 8 children are closed leaves of one value (not linked)
static inline byte collapse_octree_branch(
    void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    if (!node) {
        return 0;
    }
    void* kids = octree_children(node, type_offset);
    if (!kids) {
        return 0;
    }
    byte first_value = *(byte*)((char*) kids + value_offset);
    for (byte i = 0; i < octree_length; i++) {
        void* child = octree_child(kids, i, stride);
        // NOTE: a link or children both use ptr, so any ptr blocks the collapse
        if (*(void**) child ||
            octree_is_linked(child, type_offset) ||
            *(byte*)((char*) child + value_offset) != first_value)
        {
            return 0;
        }
    }
    *(byte*)((char*) node + value_offset) = first_value;
    zox_free(kids);
    *(void**) node = NULL;
    return 1;
}

// bottom up collapse of uniform branches, returns 1 if node itself collapsed
static inline byte reduce_octree_node(
    void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    if (!node) {
        return 0;
    }
    void* kids = octree_children(node, type_offset);
    if (!kids) {
        return 0;
    }
    for (byte i = 0; i < octree_length; i++) {
        reduce_octree_node(
            octree_child(kids, i, stride),
            stride,
            value_offset,
            type_offset);
    }
    return collapse_octree_branch(
        node,
        stride,
        value_offset,
        type_offset);
}

static inline byte is_in_octree(
    const void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset,
    byte check)
{
    if (!node) {
        return 0;
    }
    if (*(const byte*)((const char*) node + value_offset) == check) {
        return 1;
    }
    void* children = octree_children(node, type_offset);
    if (!children) {
        return 0;
    }
    for (byte i = 0; i < octree_length; i++) {
        if (is_in_octree(
            octree_child(children, i, stride),
            stride,
            value_offset,
            type_offset,
            check))
        {
            return 1;
        }
    }
    return 0;
}

// most popular value of 8 children, first found wins ties
//  - skip_air: air never wins (VoxelNode), so lods of thin solids stay visible
static inline byte octree_children_mode(
    void* kids,
    size_t stride,
    size_t value_offset,
    byte skip_air)
{
    byte child_values[octree_length];
    byte child_counts[octree_length] = { 0 };
    byte child_count = 0;
    for (byte i = 0; i < octree_length; i++) {
        byte value = *(byte*)((char*) octree_child(kids, i, stride) + value_offset);
        byte j;
        for (j = 0; j < child_count; j++) {
            if (child_values[j] == value) {
                break;
            }
        }
        if (j == child_count) {
            child_values[child_count++] = value;
        }
        child_counts[j]++;
    }
    byte max_count = 0;
    byte mode_value = 0;
    for (byte i = 0; i < child_count; i++) {
        if (skip_air && !child_values[i]) {
            continue;
        }
        if (child_counts[i] > max_count) {
            max_count = child_counts[i];
            mode_value = child_values[i];
        }
    }
    return mode_value;
}

// NOTE: Optimizes octree by picking most popular value, parents become lod values
static inline void optimize_octree_node(
    void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset,
    byte skip_air)
{
    if (!node) {
        return;
    }
    void* kids = octree_children(node, type_offset);
    if (!kids) {
        return;
    }
    for (byte i = 0; i < octree_length; i++) {
        optimize_octree_node(
            octree_child(kids, i, stride),
            stride,
            value_offset,
            type_offset,
            skip_air);
    }
    *(byte*)((char*) node + value_offset) =
        octree_children_mode(kids, stride, value_offset, skip_air);
}

// optimize + reduce in one bottom up pass, returns 1 if node itself collapsed
static inline byte optimize_reduce_octree_node(
    void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset,
    byte skip_air)
{
    if (!node) {
        return 0;
    }
    void* kids = octree_children(node, type_offset);
    if (!kids) {
        return 0;
    }
    for (byte i = 0; i < octree_length; i++) {
        optimize_reduce_octree_node(
            octree_child(kids, i, stride),
            stride,
            value_offset,
            type_offset,
            skip_air);
    }
    *(byte*)((char*) node + value_offset) =
        octree_children_mode(kids, stride, value_offset, skip_air);
    return collapse_octree_branch(
        node,
        stride,
        value_offset,
        type_offset);
}

// NOTE: Optimizes octree by picking highest value
static inline void reduce_to_max_octree_node(
    void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    if (!node) {
        return;
    }
    void* kids = octree_children(node, type_offset);
    if (!kids) {
        return;
    }
    byte max_value = 0;
    for (byte i = 0; i < octree_length; i++) {
        void* child = octree_child(kids, i, stride);
        reduce_to_max_octree_node(
            child,
            stride,
            value_offset,
            type_offset);
        byte value = *(byte*)((char*) child + value_offset);
        if (value > max_value) {
            max_value = value;
        }
    }
    *(byte*)((char*) node + value_offset) = max_value;
}

static inline void get_octree_bounds_node(
    const void* node,
    size_t stride,
    size_t value_offset,
    size_t type_offset,
    byte3 position,
    byte size,
    byte3* bounds)
{
    if (!node) {
        return;
    }
    byte value = *(const byte*)((const char*) node + value_offset);
    void* children = octree_children(node, type_offset);
    // Solid leaf fills entire region
    if (!children) {
        if (!value) {
            return;
        }
        byte3 end = byte3_add(position, byte3_single(size));
        if (end.x > bounds->x) bounds->x = end.x;
        if (end.y > bounds->y) bounds->y = end.y;
        if (end.z > bounds->z) bounds->z = end.z;
        return;
    }
    byte half = size >> 1;
    for (byte i = 0; i < octree_length; i++) {
        byte3 child_position = position;
        if (i & 1) child_position.x += half;
        if (i & 2) child_position.y += half;
        if (i & 4) child_position.z += half;
        get_octree_bounds_node(
            octree_child(children, i, stride),
            stride,
            value_offset,
            type_offset,
            child_position,
            half,
            bounds);
    }
}

static inline byte3 get_octree_bounds(
    const void* node,
    byte depth,
    size_t stride,
    size_t value_offset,
    size_t type_offset)
{
    byte3 bounds = byte3_zero;
    get_octree_bounds_node(
        node,
        stride,
        value_offset,
        type_offset,
        byte3_zero,
        (byte)(1 << depth),
        &bounds);
    return bounds;
}
