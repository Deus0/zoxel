/*
 * +----------------------------------------------------------+
 * | Zox Module: Chunks                                       |
 * |                                                          |
 * |  Generation - Octrees - Meshing - Spatial - Chunk State  |
 * |                                                          |
 * +----------------------------------------------------------+
 * */
#define zox_generate_model_run 1
#define zox_generate_model_bake 2
#define zox_generate_model_end 0
byte zox_is_slow_updates = 1;
double chunk_mesh_deactivate_delay = 0.5;
#include "com/_.c"
#include "chunks2/_.c"
#include "chunks3/_.c"

void import_chunks(ecs* world) {
    zox_module(chunks);
    zox_components_chunks(world);
    zox_add_module(chunks2);
    zox_add_module(chunks3);
}
