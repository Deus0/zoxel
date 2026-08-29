#include "lods.c"
#include "bounds.c"
#include "voctree.c"
#include "positioning.c"
#include "ground.c"

void module_dispose_chunks3(ecs *world, void *ctx) {
    // dispose_hook_on_destroyed_VoxelNode();
}





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