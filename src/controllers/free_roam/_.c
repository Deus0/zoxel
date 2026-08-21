// relies on players and cameras
#include "dat/settings.c"
#include "sys/_.c"

void import_freeroam(ecs* world) {
    zox_module(freeroam);
    define_systems_free_roam(world);
}
