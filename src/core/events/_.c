// TODO: Pretty much depreciated this - maybe take events out of timing
#include "dat/_.c"
#include "mcr/names.c"
#include "fun/_.c"
#include "pre/_.c"

void import_events(ecs* world) {
    zox_module(events);
    add_hook_spawn_prefabs(spawn_prefabs_generic);
}