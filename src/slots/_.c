#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"

void import_slots(ecs* world) {
    zox_module(slots);
    zox_components_slots(world);
    add_hook_spawn_prefabs(zox_spawn_prefabs_slots);
}
