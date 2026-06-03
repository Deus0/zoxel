// Vodes3 are our Voxel Entities in the World!
#ifndef zoxm_vodes3
#define zoxm_vodes3

byte block_vox_render_at_lod = 0;
#include "hok/_.c"
#include "sys/_.c"
#include "fun/_.c"

void module_dispose_vodes3(ecs *world, void *ctx) {
    (void) world;
    (void) ctx;
    dispose_hook_spawned_block();
}

zox_begin_module(Vodes3) {
    define_systems_vodes3(world);
    initialize_hook_spawned_block();
    zox_module_dispose(module_dispose_vodes3);
    // add_hook_spawned_block(&spawned_block_vox);
} zox_end_module(Vodes3);

#endif
