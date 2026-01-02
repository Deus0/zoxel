#if !defined(zoxm_debug) && defined(zoxm_players)
#define zoxm_debug

#include "dat/settings.c"
#include "fun/amd.c"
#include "fun/system.c"
#include "sys/game_debug_label_system.c"

void dispose_debug(ecs* world, void* ctx) {
    (void) world;
    (void) ctx;
    dispose_amd();
}

zox_begin_module(Debug)
    // zox_system(GameDebugLabelSystem, EcsOnStore, [out] texts.TextDirty, [out] texts.TextData, [none] game.u.i.GameDebugLabel)
    initialize_amd();
    zox_module_dispose(dispose_debug)
zox_end_module(Debug)

#endif
