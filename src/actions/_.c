/*
 * +------------------------------------------------------------------+
 * | Zox Module: Actions                                              |
 * |                                                                  |
 * |  Skills - Items - Actionbar - Activation - Action State          |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
#if !defined(zoxm_actions) && defined(zoxm_items) && defined(zoxm_skills)
#define zoxm_actions

byte test_actions_blocks = 0;
byte test_actions_skills = 0;
// zoxc_userdata(Action);
#include "com/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ui/_.c"
#include "sys/_.c"

zox_begin_module(Actions) {
    zox_define_components_actions(world);
    zox_define_systems_actions(world);
    add_hook_spawn_prefabs(spawn_prefabs_actions);
    zox_import_module(ActionsUI);
} zox_end_module(Actions);

#endif
