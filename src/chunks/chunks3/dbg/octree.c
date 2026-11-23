// debug: an octree drawn as line cubes, any octree type (a function, not a macro a type)
//  - a cube for each node at target_depth, or a leaf above it, coloured by its value (debug_octree_style's color)
//  - nodes whose value is ignore (-1 for none) aren't drawn
//  - with a compare tree (a chunk's VoxelNode) and skip_solid, only where its voxel is air
//  - children only through octree_children, so a linked node (a block vox's) is never walked into
typedef color_rgb (*octree_debug_color)(byte value);

static inline color_rgb octree_debug_grey(byte value) {
    return (color_rgb) { value, value, value };
}

typedef struct {
    size_t stride, type_offset;                 // the tree's node size and type offset (sizeof(T), octree_type_offset_T)
    size_t compare_stride, compare_type_offset;
    byte target_depth;
    int ignore;
    byte skip_solid;
    octree_debug_color color;
} debug_octree_style;

float debug_octree_padding = 0.9f;
float debug_octree_thickness = 4;

static void debug_octree_node(ecs* world, const debug_octree_style* style, const void* node, const void* compare,
        byte depth, float3 position, float scale) {
    const byte* kids = depth < style->target_depth ? octree_children(node, style->type_offset) : NULL;
    if (!kids) {
        const byte value = *(const byte*)((const char*) node + octree_value_offset);
        if (value == style->ignore || (style->skip_solid && compare &&
            *(const byte*)((const char*) compare + octree_value_offset))) {
            return;
        }
        const float3 center = float3_add(position, float3_single(scale * 0.5f));
        debug_cubec(world, center, float3_single(scale * debug_octree_padding), style->color(value), debug_octree_thickness);
        return;
    }
    const byte* compare_kids = compare ? octree_children(compare, style->compare_type_offset) : NULL;
    const float half = scale * 0.5f;
    for (byte i = 0; i < octree_length; i++) {
        const byte3 o = octree_positions[i];
        debug_octree_node(world, style, kids + i * style->stride, compare_kids ? compare_kids + i * style->compare_stride : compare,
            depth + 1, float3_add(position, (float3) { o.x * half, o.y * half, o.z * half }), half);
    }
}

// a tree whose root's lower corner is at position, scale wide
static inline void debug_octree(ecs* world, const debug_octree_style* style, const void* root, const void* compare,
        float3 position, float scale) {
    debug_octree_node(world, style, root, compare, 0, position, scale);
}
