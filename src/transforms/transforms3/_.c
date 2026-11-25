
uint transforms3_children_capacity = 251;
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "sys/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_transforms3(ecs* world) {
    zox_module(transforms3);
    zox_components_transforms3(world);
    zox_systems_transforms3(world);
    add_hook_spawn_prefabs(zox_prefabs_transforms3);
#ifdef zox_tests
    zox_tsts_transforms3();
#endif
}
