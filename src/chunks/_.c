/*
 *  Zox Chunks
 *
 *      - Using Octrees to create Voxel Meshes
 *
 * */
#ifndef zoxm_chunks
#define zoxm_chunks

byte zox_is_slow_updates = 1;
#include "com/_.c"
#include "chunks2/_.c"
#include "chunks3/_.c"

zox_begin_module(Chunks) {
    define_components_chunks(world);
    zox_import_module(Chunks2);
    zox_import_module(Chunks3);
} zox_end_module(Chunks);

#endif
