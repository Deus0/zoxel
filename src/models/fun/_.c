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

// graphs
#include "nodegraph.c"
// nodes
#include "paint.c"
#include "cube.c"
#include "sphere.c"
#include "ellipsoid.c"
#include "cylinder.c"
// Misc
#include "slime.c"