/*
 * +-------------------------------------------------------+
 * | Zox Module: Skills                                    |
 * |                                                       |
 * |  Abilities - Effects - Cooldowns - Skill Trees        |
 * |                                                       |
 * +-------------------------------------------------------+
 */
#include "set/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "eve/_.c"
#include "ui/_.c"
#include "auras/_.c"
#include "tst/_.c"

void import_skills(ecs* world) {
    zox_module(skills);
    zox_components_skills(world);
    zox_systems_skills(world);
    add_hook_spawn_prefabs(spawn_prefabs_skills);
    zox_systems_blackplague(world);
    add_hook_terminal_command(process_arguments_skills);
    zox_events_skills(world);
    zox_add_module(skills_ui);
    zox_add_module(auras);
#ifdef zox_tests
    zox_tsts_aura_particles(world);
    zox_tsts_punch_wind_up(world);
    zox_tst_add("block_break_player", play_break_ready, play_break_check, 240);
    zox_tst_set_setup("block_break_player", play_break_setup, 0);
    zox_tst_add("block_break", block_break_ready, block_break_check, 180);
    zox_tst_set_setup("block_break", NULL, 1);
    zox_tst_add("block_break_streaming", block_break_ready, block_break_check, 300);
    zox_tst_set_setup("block_break_streaming", moving_break_setup, 1);
#endif
}
