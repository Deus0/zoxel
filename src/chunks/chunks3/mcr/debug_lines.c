#define create_octree_line_debugger(T) \
static void debug_octree_node_##T( \
    ecs *world, \
    const T *node, \
    byte depth, \
    byte target_depth, \
    float3 position, \
    float scale, \
    byte ignore_value \
) { \
    if (!node) return; \
    \
    const void *const* kids_ptr = (const void *const*)node; \
    if (depth == target_depth || !*kids_ptr) { \
        /* Skip nodes with ignored value */ \
        if (node->value == ignore_value) return; \
        \
        /* Draw cube for node at target depth */ \
        byte v = node->value; \
        color_rgb col = { v, v, v }; \
        float3 size = float3_single(scale * 0.95f); /* padding */ \
        float3 center = float3_add(position, float3_half(size)); \
        debug_cubec(world, center, size, col); \
        return; \
    } \
    \
    /* Descend toward target depth */ \
    if (!*kids_ptr) return; \
    \
    float child_scale = scale * 0.5f; \
    for (byte i = 0; i < 8; i++) { \
        const T *child = (const T*)((const char*)(*kids_ptr) + i * sizeof(T)); \
        if (!child) continue; \
        byte3 offset = octree_positions_b[i]; \
        float3 child_pos = float3_add(position, \
            (float3){offset.x * child_scale, offset.y * child_scale, offset.z * child_scale}); \
        debug_octree_node_##T(world, child, depth+1, target_depth, child_pos, child_scale, ignore_value); \
    } \
} \
\
static inline void debug_octree_##T( \
    ecs *world, \
    const T *root, \
    byte target_depth, \
    float3 root_position, \
    float root_scale, \
    byte ignore_value \
) { \
    debug_octree_node_##T( \
        world, root, 0, target_depth, root_position, root_scale, ignore_value \
    ); \
}
