/*
 * +------------------------------------------------------+
 * | Zox Module: UI                                       |
 * |                                                      |
 * |  Panels - Labels - Buttons - Layouts - Interface     |
 * |                                                      |
 * |  Uses: Transforms2 - Layout2 - Interaction           |
 * |                                                      |
 * +------------------------------------------------------+
 **/
byte is_log_dragging = 0;
#include "set/_.c"
#include "com/_.c"
#include "dat/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "interaction/_.c"
#include "glyphs/_.c"
#include "texts/_.c"
#include "tooltips/_.c"
#include "elements2/_.c"
#include "elements3/_.c"
#include "windows/_.c"
#include "navigation/_.c"
#include "touch/_.c"
#include "render_textures/_.c"
#include "containers/_.c"
#include "dbg/_.c"
#include "tst/_.c"

void import_ui(ecs* world) {
    zox_module(ui);
    zox_components_elements(world);
    zox_systems_elements(world);
    add_hook_terminal_command(arguments_ui);
    add_hook_spawn_prefabs(initialize_settings_elements);
    add_hook_spawn_prefabs(spawn_prefabs_elements);
    zox_add_module(glyphs);
    zox_add_module(texts);
    zox_add_module(interactions);
    zox_add_module(tooltips);
    zox_add_module(elements2);
    zox_add_module(elements3);
    zox_add_module(windows);
    zox_add_module(navigation);
    zox_add_module(touch);
    zox_add_module(render_textures);
    zox_add_module(ui_containers);
}
