// this is used for visually representing data
#include "com/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/time.c"

// TODO: Move into UIs
void import_plots(ecs* world) {
    zox_module(plots);
    add_hook_spawn_prefabs(spawn_prefabs_plots);
    zox_components_plots(world);
    zox_systems_plots(world);
}
