#ifndef zoxm_events
#define zoxm_events

// TODO: Pretty much depreciated this - maybe take events out of timing

#include "dat/_.c"
#include "mcr/names.c"
#include "fun/_.c"
#include "pre/_.c"

zox_begin_module(Events)
    add_hook_spawn_prefabs(spawn_prefabs_generic);
zox_end_module(Events)

#endif