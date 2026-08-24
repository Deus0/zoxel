/*
 * +------------------------------------------------------------------+
 * | Zox Module: Streaming                                            |
 * |                                                                  |
 * |  Position Change Detection - Spawn Queue                         |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_streaming(ecs* world) {
    zox_module(streaming);
    define_components_streaming(world);
    define_systems_streaming(world);
    // initialize_settings_streaming(world);
    add_hook_spawn_prefabs(zox_define_prefabs_streaming);
}
