
#include "dat/settings.c"
#include "com/_.c"
#include "pre/prefabs.c"
#include "fun/neuron_util.c"
#include "fun/build_test_brain.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_neurals(ecs* world) {
    zox_module(neurals);
    define_components_neurals(world);
    define_systems_neurals(world);
    add_hook_spawn_prefabs(spawn_prefabs_neurals);
    // spawn_test_brain(world);
}
