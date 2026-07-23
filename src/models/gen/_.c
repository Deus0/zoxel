static inline bool in_bounds(byte3 p, byte size) {
    return p.x < size && p.y < size && p.z < size;
}
// TODO: Move to octree macros
static inline void set_voxel_safe(VoxelNode *tree, byte depth, byte3 p, byte v) {
    if (in_bounds(p, octree_size(depth))) {
        set_VoxelNode(tree, depth, p, v);
    } else {
        zox_logw("Position [%ix%ix%i] out of B [%i]", p.x, p.y, p.z, depth);
    }
}

#include "noisey.c"
#include "soil.c"
#include "blended.c"
#include "rubble.c"
#include "bricks.c"
#include "wood.c"
#include "flowers.c"
#include "orientation.c"
#include "road.c"
