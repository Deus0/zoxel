// Tree Traversal Functions

static inline byte get_octree_max_value(const void* node, size_t stride, size_t value_offset) {
    if (!node) {
        return 0;
    }
    byte max = *(const byte*)((const char*) node + value_offset);
    const void* children = *(const void* const*) node;
    if (!children) {
        return max;
    }
    for (byte i = 0; i < 8; i++) {
        const void* child = (const char*) children + (i * stride);
        byte new_max = get_octree_max_value(child, stride, value_offset);
        if (new_max > max) {
            max = new_max;
        }
    }
    return max;
}

static inline byte reduce_octree_node(void* node, size_t stride, size_t value_offset, size_t type_offset) {
    if (!node) {
        return 0;
    }
    void** ptr = (void**) node;
    if (type_offset) {
        byte type = *(byte*)((char*)node + type_offset);
        if (type == node_type_instance) {
            return 0;
        }
    }
    void* kids = *ptr;
    // No children, nothing to reduce
    if (!kids) {
        return 0;
    }
    // Recurse bottom-up
    for (byte i = 0; i < 8; i++) {
        reduce_octree_node((char*) kids + i * stride, stride, value_offset, type_offset);
    }
    // Check uniformity
    byte first_val = *(byte*)((char*) kids + value_offset);
    bool uniform = true;
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        // NOTE: Don't collapse nodes with entity links
        if (type_offset) {
            byte type = *(byte*)((char*)child + type_offset);
            if (type == node_type_instance) {
                uniform = 0;
                break;
            }
        }
        byte val = *(byte*)((char*) child + value_offset);
        void* subkids = *(void**) child;
        if (val != first_val || subkids != NULL) {
            uniform = 0;
            break;
        }
    }
    // Collapse if uniform
    if (uniform) {
        *(byte*)((char*) node + value_offset) = first_val;
        zox_free(kids);
        *ptr = NULL;
        return 1;
    }
    return 0;
}

static inline byte collapse_octree_branch(void* node, size_t stride, size_t value_offset) {
    if (!node) {
        return 0;
    }
    void** ptr = (void**)node;
    void* kids = *ptr;
    if (!kids) {
        return 0;
    }
    // Check if all children match first value and have no subchildren
    byte first_val = *(byte*)((char*)kids + value_offset);
    for (byte i = 0; i < 8; i++) {
        void* child = (char*)kids + i * stride;
        byte val = *(byte*)((char*)child + value_offset);
        void* subkids = *(void**)child;
        if (val != first_val || subkids != NULL) {
            return 0; // can't collapse
        }
    }
    // Collapse
    *(byte*)((char*)node + value_offset) = first_val;
    zox_free(kids);
    *ptr = NULL;
    return 1;
}

static inline byte is_in_octree(const void* node, size_t stride, size_t value_offset, byte check) {
    if (!node) {
        return 0;
    }
    byte value = *(const byte*)((const char*) node + value_offset);
    if (value == check) {
        return 1;
    }
    const void* children = *(const void* const*) node;
    if (!children) {
        return 0;
    }
    for (byte i = 0; i < 8; i++) {
        const void* child = (const char*) children + (i * stride);
        byte is_in = is_in_octree(child, stride, value_offset, check);
        if (is_in) {
            return 1;
        }
    }
    return 0;
}

// NOTE: Optimizes octree by picking most popular value
static inline void optimize_octree_node(void* node, size_t stride, size_t value_offset, size_t type_offset) {
    if (!node) {
        return;
    }
    void** ptr = (void**) node;
    void* kids = *ptr;
    // Leaf node, nothing to optimize
    if (!kids) {
        return;
    }
    if (type_offset) {
        byte type = *(byte*)((char*) node + type_offset);
        if (type == node_type_instance) {
            return;
        }
    }
    // Recurse bottom-up
    for (byte i = 0; i < 8; i++) {
        optimize_octree_node((char*) kids + i * stride, stride, value_offset, type_offset);
    }
    // Count child values (max 8)
    byte child_vals[8];
    byte child_count = 0;
    byte child_counts[8] = { 0 };
    // Track unique values and counts
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        byte value = *(byte*)((char*) child + value_offset);
        byte j;
        for (j = 0; j < child_count; j++) {
            if (child_vals[j] == value) {
                break;
            }
        }
        if (j == child_count) {
            child_vals[child_count] = value;
            child_count++;
        }
    }
    // Simple frequency array for mode (up to 8 values)
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        byte value = *(byte*)((char*) child + value_offset);
        for (byte j = 0; j < child_count; j++) {
            if (child_vals[j] == value) {
                child_counts[j]++;
                break;
            }
        }
    }
    // Find mode
    byte max_count = 0;
    byte mode_val = 0;
    for (byte i = 0; i < child_count; i++) {
        byte value = child_vals[i];
        byte count = child_counts[i];
        // skip air if VoxelOctree
        if (type_offset && !value) {
            continue;
        }
        if (count > max_count) {
            max_count = count;
            mode_val = value;
        }
    }
    // Set parent value
    *(byte*)((char*) node + value_offset) = mode_val;
}

// NOTE: Optimizes octree by picking highest value
static inline void reduce_to_max_octree_node(void* node, size_t stride, size_t value_offset, size_t type_offset) {
    if (!node) {
        return;
    }
    void** ptr = (void**) node;
    void* kids = *ptr;
    // Leaf node, nothing to optimize
    if (!kids) {
        return;
    }
    if (type_offset) {
        byte type = *(byte*)((char*) node + type_offset);
        if (type == node_type_instance) {
            return;
        }
    }
    // Recurse bottom-up
    for (byte i = 0; i < 8; i++) {
        reduce_to_max_octree_node((char*) kids + i * stride, stride, value_offset, type_offset);
    }
    // Find highest child value
    byte max_val = 0;
    for (byte i = 0; i < 8; i++) {
        void* child = (char*) kids + i * stride;
        byte value = *(byte*)((char*) child + value_offset);
        // skip air if VoxelOctree
        if (type_offset && !value) {
            continue;
        }
        if (value > max_val) {
            max_val = value;
        }
    }
    // Set parent value
    *(byte*)((char*) node + value_offset) = max_val;
}

static inline void get_octree_bounds_node(
    const void* node,
    size_t stride,
    size_t value_offset,
    byte3 position,
    byte size,
    byte3* bounds)
{
    if (!node) {
        return;
    }
    byte value = *(const byte*)((const char*)node + value_offset);
    const void* children = *(const void* const*)node;
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
    for (byte i = 0; i < 8; i++) {
        byte3 child_position = position;
        if (i & 1) child_position.x += half;
        if (i & 2) child_position.y += half;
        if (i & 4) child_position.z += half;
        const void* child = (const char*)children + i * stride;
        get_octree_bounds_node(
            child,
            stride,
            value_offset,
            child_position,
            half,
            bounds);
    }
}

static inline byte3 get_octree_bounds(
    const void* node,
    byte depth,
    size_t stride,
    size_t value_offset)
{
    byte3 bounds = byte3_zero;
    get_octree_bounds_node(
        node,
        stride,
        value_offset,
        byte3_zero,
        (byte)(1 << depth),
        &bounds);
    return bounds;
}