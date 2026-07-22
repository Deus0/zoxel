/*
 *  Zox Chunks
 *
 *      - Using Octrees to create Voxel Meshes
 *
 * */
#ifndef zoxm_chunks
#define zoxm_chunks

#define zox_generate_model_run 1
#define zox_generate_model_bake 2
#define zox_generate_model_end 0
byte zox_is_slow_updates = 1;
double chunk_mesh_deactivate_delay = 0.5;
#include "com/_.c"
#include "chunks2/_.c"
#include "chunks3/_.c"

zox_begin_module(Chunks) {
    define_components_chunks(world);
    zox_import_module(Chunks2);
    zox_import_module(Chunks3);
} zox_end_module(Chunks);

#endif
