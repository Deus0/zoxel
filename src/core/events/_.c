// TODO: Pretty much depreciated this - maybe take events out of timing
#include "com/_.c"
#include "dat/_.c"
#include "mcr/names.c"
#include "fun/_.c"
#include "pre/_.c"
#include "sys/_.c"

void import_events(ecs* world) {
    zox_module(events);
    zox_components_events(world);
    zox_systems_events(world);
    add_hook_spawn_prefabs(spawn_prefabs_generic);
}
