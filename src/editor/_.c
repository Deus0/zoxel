#ifndef zoxm_editor
#define zoxm_editor

DebugLabelEvent local_debug_label;

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

// TODO: make basic structs just to read out generic component data
// TODO: add_entity_to_labels should be a macro that takes in Tag so I can filter it with zox_has before adding: try these - Element, Character, Camera, Position3D, Position2,

#endif
