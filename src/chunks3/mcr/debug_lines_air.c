#define create_octree_line_debugger_compare(T, T2) \
static void debug_octree_node_compare_##T( \
    ecs *world, \
    const T *node, \
    const T2 *compare_node, \
    byte depth, \
    byte target_depth, \
    float3 position, \
    float scale, \
    byte ignore_value \
) { \
    if (!node) return; \
    \
    const void** kids_ptr = (const void**)node; \
    if (depth == target_depth || !*kids_ptr) { \
        /* Draw cube for node at target depth */ \
        byte v = node->value; \
        color_rgb col = { v, v, v }; \
        float3 size = float3_single(scale * 0.95f); /* padding */ \
        float3 center = float3_add(position, float3_half(size)); \
        debug_cubec(world, center, size, col); \
        return; \
    } \
    \
    float child_scale = scale * 0.5f; \
    for (byte i = 0; i < 8; i++) { \
        const T *child = (const T*)((const char*)(*kids_ptr) + i * sizeof(T)); \
        if (!child) continue; \
        byte3 offset = octree_positions_b[i]; \
        float3 child_pos = float3_add(position, \
            (float3){offset.x * child_scale, offset.y * child_scale, offset.z * child_scale}); \
        \
        const T2* child_compare = compare_node; \
        if (compare_node) { \
            const void *const* compare_kids = (const void *const*)compare_node; \
            if (compare_kids && *compare_kids) { \
                child_compare = (const T2*)((const char*)(*compare_kids) + i * sizeof(T2)); \
            } \
        } \
        \
        /* Skip nodes with ignored value */ \
        if (child->value == ignore_value) continue; \
        \
        /* Skip if compare_node indicates underground */ \
        if (child_compare && child_compare->value) continue; \
        \
        debug_octree_node_compare_##T(world, child, child_compare, depth+1, target_depth, child_pos, child_scale, ignore_value); \
    } \
} \
\
static inline void debug_octree_compare_##T( \
    ecs *world, \
    const T *root, \
    const T2 *compare_root, \
    byte target_depth, \
    float3 root_position, \
    float root_scale, \
    byte ignore_value \
) { \
    debug_octree_node_compare_##T( \
        world, root, compare_root, 0, target_depth, root_position, root_scale, ignore_value \
    ); \
}
