#ifndef zoxm_chunks
#define zoxm_chunks


// TODO: Use BlockCanGroup for reduction - atm grass etc can reduce

#include "com/_.c"
#include "chunks2/_.c"
#include "chunks3/_.c"

// TODO: Move to Models module
byte block_vox_depth = 5;

static inline byte camera_distance_to_block_vox_depth(byte distance) {
    return camera_distance_to_render_depth(distance, block_vox_depth, vox_lod_near, terrain_lod_near);
}


zox_begin_module(Chunks)
    define_components_chunks(world);
    zox_import_module(Chunks2);
    zox_import_module(Chunks3);
zox_end_module(Chunks)

#endif
