#include "chunk_lod.c"
#include "prefabs.c"
#include "lods.c"
#include "distance.c"
#include "bounds.c"
#include "voxel_get.c"
#include "voxel_set.c"
#include "voxel_optimize.c"
#include "positionl.c"
#include "positionv.c"
#include "positionc.c"
#include "positioning.c"
#include "ground_finder.c"
#include "chunk_entity_links.c"
#include "debug.c"
#include "settings.c"
#include "paint.c"

static inline byte camera_distance_to_block_vox_depth(byte distance) {
    return camera_distance_to_render_depth(distance, block_vox_depth, vox_lod_near, terrain_lod_near);
}
