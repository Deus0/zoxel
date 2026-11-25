
// TODO: Layout can just effect local position
//          - Then we can modify transform easier of ui
#include "com/_.c"
#include "fun/_.c"
#include "sys/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_transforms2(ecs* world) {
    zox_module(transforms2);
    zox_components_transforms2(world);
    zox_systems_transforms2(world);
#ifdef zox_tests
    zox_tsts_transforms2();
#endif
}
