#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"

void import_interactions(ecs* world) {
    zox_module(interactions);
    zox_components_interaction(world);
    zox_systems_interaction(world);
    add_hook_spawn_prefabs(zox_prefabs_interaction);
}
