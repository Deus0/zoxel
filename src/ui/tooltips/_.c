byte zox_log_tooltips = 0;
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#ifdef zox_tests
#include "tst/gone.c"
#endif

void import_tooltips(ecs* world) {
    zox_module(tooltips);
    zox_components_tooltips(world);
    zox_systems_tooltips(world);
#ifdef zox_tests
    zox_tsts_tooltip_gone(world);
#endif
}
