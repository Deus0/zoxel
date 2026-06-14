#ifndef zoxm_streaming
#define zoxm_streaming

#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Streaming) {
    define_components_streaming(world);
    define_systems_streaming(world);
    initialize_settings_streaming(world);
    add_hook_spawn_prefabs(zox_define_prefabs_streaming);
} zox_end_module(Streaming);

#endif
