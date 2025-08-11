#include "chunk_lod.c"
#include "prefabs.c"
#include "lods.c"
#include "distance.c"
#include "bounds.c"
#include "validation.c"
#include "voxel_get.c"
#include "voxel_set.c"
#include "voxel_optimize.c"
#include "positioning.c"
#include "ground_finder.c"
#include "chunk_entity_links.c"
#include "debug.c"
#include "settings.c"

// add here
static inline float get_chunk_scale(byte node_depth, byte terrain_depth, float terrain_voxel_scale) {
    byte ddepth = terrain_depth - node_depth;   // assuming node depth is lower than terrain (max depth)
    return terrain_voxel_scale * powers_of_two[ddepth];  // we multiply by the depth difference power - if 2 = 2*2 = 4, 0.5 becomes 2 in size
}