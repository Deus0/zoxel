static inline bool in_bounds(byte3 p, byte size) {
    return p.x < size && p.y < size && p.z < size;
}

// TODO: Move to octree macros
static inline void set_voxel_safe(VoxelNode *tree, byte depth, byte3 p, byte v) {
    if (!in_bounds(p, octree_size(depth))) {
        // zox_logw("Position [%ix%ix%i] out of B [%i]", p.x, p.y, p.z, depth);
        return;
    }
    set_VoxelNode(tree, depth, p, v);
}

// Shifts our node to the new blueprint size
static inline void scale_node_transform(
    byte3 bounds,
    byte3* position,
    byte3* size)
{
    uint blueprint_length = octree_size(nodegraph_max_depth);
    position->x = (position->x * bounds.x) / blueprint_length;
    position->y = (position->y * bounds.y) / blueprint_length;
    position->z = (position->z * bounds.z) / blueprint_length;
    size->x = (size->x * bounds.x) / blueprint_length;
    size->y = (size->y * bounds.y) / blueprint_length;
    size->z = (size->z * bounds.z) / blueprint_length;
    if (!size->x) size->x = 1;
    if (!size->y) size->y = 1;
    if (!size->z) size->z = 1;
}

// Scales a node transform from the fixed nodegraph depth to the target model depth.
// Since octree sizes are powers of two, this is an exact bit shift.
static inline void shift_node_transform(byte model_depth, byte3* position, byte3* size) {
    byte node_depth = nodegraph_max_depth;
    int shift = (int)model_depth - (int)nodegraph_max_depth;
    if (shift > 0) {
        position->x <<= shift;
        position->y <<= shift;
        position->z <<= shift;
        size->x <<= shift;
        size->y <<= shift;
        size->z <<= shift;
    } else if (shift < 0) {
        byte shift = node_depth - model_depth;
        position->x >>= shift;
        position->y >>= shift;
        position->z >>= shift;
        size->x >>= shift;
        size->y >>= shift;
        size->z >>= shift;
    }
    if (!size->x) size->x = 1;
    if (!size->y) size->y = 1;
    if (!size->z) size->z = 1;
}

// graphs
#include "nodegraph.c"
// nodes
#include "paint.c"
#include "cube.c"
#include "sphere.c"
#include "ellipsoid.c"
#include "cylinder.c"