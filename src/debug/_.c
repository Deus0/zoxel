#if !defined(zoxm_debug) && defined(zoxm_players)
#define zoxm_debug

// TODO: Remove this module, just use module dbg folders
#include "fun/_.c"
#include "sys/_.c"

void dispose_debug(ecs* world, void* ctx) {
    dispose_amd();
}

zox_begin_module(Debug) {
    initialize_amd();
    zox_module_dispose(dispose_debug);
} zox_end_module(Debug);

#endif
