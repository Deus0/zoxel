#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_interaction(ecs* world) {
    zox_module(interaction);
    zox_components_interaction(world);
    zox_systems_interaction(world);
    add_hook_spawn_prefabs(zox_spawn_prefabs_elements_interaction);
}
