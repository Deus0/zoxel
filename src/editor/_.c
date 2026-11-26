/*
 * +------------------------------------------------------+
 * | Zox Module: Editor                                   |
 * |                                                      |
 * |  Tools - Inspectors - Gizmos - Debugging - Editing   |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *  Keys (debug player):
 *  - y: hierarchy, the entity tree, click a row to inspect it
 *  - n: files, browse resources and saves and inspect file metadata
 *  - u: inspector, an entity's components, click an entity component to follow it
 *  - z: fps display (the "FPS" setting, in release builds too: the only editor ui there)
 *  - x: profiler (frame times)
 *  - c: debug label, set its text with set_prefab_debug_label
 *  - inspect_entity: points an open inspector at an entity (used by debug spawners)
 *  - test: bash bsh/tst.sh editor
 */
DebugLabelEvent local_debug_label;
byte zox_fps_display = 0;       // setting "FPS": each player's fps display, top right (fps_display_show_system)
byte editor_overlay_layer = 232;
double time_update_debug_label_system_rate = 5;
#include "set/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "dbg/_.c"
#include "eve/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_editor(ecs* world) {
    zox_module(editor);
    zox_components_editor(world);
    zox_systems_editor(world);
    add_hook_spawn_prefabs(spawn_prefabs_editor);
    add_hook_spawn_prefabs(zox_events_editor);
#ifdef zox_debug
    add_hook_key_down(toggle_fps_viewer);
    add_hook_key_down(toggle_inspector);
    add_hook_key_down(toggle_hierarchy);
    add_hook_key_down(toggle_files);
#endif
#ifdef zox_tests
    zox_tsts_editor();
#endif
}
