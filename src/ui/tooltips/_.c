
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

void import_tooltips(ecs* world) {
    zox_module(tooltips);
    zox_components_tooltips(world);
    zox_systems_tooltips(world);
}