#ifndef zoxm_editor
#define zoxm_editor

DebugLabelEvent local_debug_label;
byte editor_overlay_layer = 232;
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(Editor) {
    define_components_editor(world);
    define_systems_editor(world);
    add_hook_spawn_prefabs(spawn_prefabs_editor);
} zox_end_module(Editor);

#endif
