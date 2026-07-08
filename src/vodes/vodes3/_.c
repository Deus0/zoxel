// Vodes3 are our Voxel Entities in the World!
#ifndef zoxm_vodes3
#define zoxm_vodes3

float block_damage_overlay_buffer = 1.08f;
byte block_vox_render_at_lod = 0;
#include "com/_.c"
#include "hok/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"

void module_dispose_vodes3(ecs *world, void *ctx) {
    dispose_hook_spawned_block();
}

zox_begin_module(Vodes3) {
    zox_define_components_vodes3(world);
    define_systems_vodes3(world);
    initialize_hook_spawned_block();
    zox_module_dispose(module_dispose_vodes3);
    add_hook_spawn_prefabs(zox_define_prefabs_vodes3);
} zox_end_module(Vodes3);

#endif
