
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"

void import_auras(ecs* world) {
    zox_module(auras);
    zox_components_auras(world);
    zox_systems_auras(world);
    add_hook_spawn_prefabs(spawn_prefabs_auras);
}
