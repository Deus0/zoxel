#include "lods.c"
#include "bounds.c"
#include "voctree.c"
#include "positioning.c"
#include "ground.c"
#include "unlit.c"
#include "ao6.c"
#include "ao27.c"

// NOTE: Assumes node depth is lower than terrain (max depth)
static inline float get_chunk_scale(
    byte chunk_depth,
    byte terrain_depth,
    float terrain_scale)
{
    if (terrain_depth < chunk_depth) {
        return terrain_scale;
    } else {
        // we multiply by the depth difference power
        // - if 2 = 2*2 = 4, 0.5 becomes 2 in size
        byte ddepth = terrain_depth - chunk_depth;
        return terrain_scale * octree_size(ddepth);
    }
}

static inline void initialize_voxel_lock(
    ecs* world,
    entity e)
{
    spinlock vlock;
    spinlock_init(&vlock);
    zox_setv(e, VoxelNodeLock, vlock);
}
