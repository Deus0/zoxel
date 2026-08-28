
// TODO: Remove this module, just use module dbg folders
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"

void dispose_debug(ecs* world, void* ctx) {
    dispose_amd();
}

void import_debug(ecs* world) {
    zox_module(debug);
    initialize_amd();
    zox_module_dispose(dispose_debug);
    add_hook_key_down(zox_dbg_test_window_world);
    add_hook_key_down(zox_dbg_test_window_uis);
    add_hook_key_down(zox_dbg_test_window_uis3);
    add_hook_key_down(zox_dbg_ui_cheats);
    add_hook_key_down(zox_dbg_ui_manual_tests);
    add_hook_key_down(zox_dbg_ui_gizmos);
    add_hook_key_down(zox_dbg_ui_overlays);
}