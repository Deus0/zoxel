/*
 * +------------------------------------------------------------------+
 * | Zox Module: Actions                                              |
 * |                                                                  |
 * |  Skills - Items - Actionbar - Activation - Action State          |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
byte test_actions_blocks = 0;
byte test_actions_skills = 0;
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ui/_.c"
#include "sys/_.c"

void import_actions(ecs* world) {
    zox_module(actions);
    zox_define_components_actions(world);
    zox_define_systems_actions(world);
    add_hook_spawn_prefabs(spawn_prefabs_actions);
    zox_add_module(actions_ui);
}