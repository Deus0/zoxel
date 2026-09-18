
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"

void import_chunks2(ecs* world) {
    zox_module(chunks2);
    zox_components_chunks2(world);
    add_hook_spawn_prefabs(spawn_prefabs_chunks2);
}
