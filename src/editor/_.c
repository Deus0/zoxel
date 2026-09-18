/*
 * +------------------------------------------------------+
 * | Zox Module: Editor                                   |
 * |                                                      |
 * |  Tools - Inspectors - Gizmos - Debugging - Editing   |
 * |                                                      |
 * +------------------------------------------------------+
 */
DebugLabelEvent local_debug_label;
byte editor_overlay_layer = 232;
double time_update_debug_label_system_rate = 5;
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void import_editor(ecs* world) {
    zox_module(editor);
    zox_components_editor(world);
    zox_systems_editor(world);
    add_hook_spawn_prefabs(spawn_prefabs_editor);
}