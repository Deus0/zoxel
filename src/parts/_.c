/*
 * +------------------------------------------------------+
 * | Zox Module: Parts                                    |
 * |                                                      |
 * |  Combine Items - Body - Gear - UI                    |
 * |                                                      |
 * +------------------------------------------------------+
 *
 *  Used By
 *
 *      - Characters
 *
 *  TODO
 *
 *      - Support for multi chunk voxes
 *      - make use TargetChunkLod
 *          - and load/unload depth based on that
 *
*/
#define body_anchor_core 0
#define body_anchor_top 1
#define body_anchor_bottom 2
#define body_anchor_right 3
#define body_anchor_left 4
#define body_anchor_back 5
#define body_anchor_forward 6

#include "com/_.c"
#include "sta/_.c"
#include "fun/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "dbg/_.c"
#include "sys/_.c"

void import_parts(ecs* world) {
    zox_module(parts);
    zox_components_bodys(world);
    zox_systems_bodys(world);
    add_hook_spawn_prefabs(spawn_prefabs_bodys);
    add_taskbar_button(world, (TaskbarData) {
        .index = 6,
        .spawn = &spawn_player_menu_body,
        .component_id = zox_id(MenuBody),
        .texture_name = "taskbar_body",
        .tooltip_text = "Body"
    });
}
