// Vodes3 are our Voxel Entities in the World!

byte block_vox_render_at_lod = 0;
#include "com/_.c"
#include "hok/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "eve/_.c"

void module_dispose_vodes3(ecs *world, void *ctx) {
    dispose_hook_spawned_block();
}

void import_vodes3(ecs* world) {
    zox_module(vodes3);
    zox_define_components_vodes3(world);
    define_systems_vodes3(world);
    initialize_hook_spawned_block();
    zox_module_dispose(module_dispose_vodes3);
    add_hook_spawn_prefabs(zox_prefabs_vodes3);
    add_hook_spawn_prefabs(zox_events_vodes3);
}
