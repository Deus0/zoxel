/*
 * +----------------------------------------------------------+
 * | Zox Module: Quests                                       |
 * |                                                          |
 * |  Objectives - Progress - Rewards - Quest State - Chains  |
 * |                                                          |
 * +----------------------------------------------------------+
 *
*  TODO
*
*       - Quest Objective: Find NPC named Bob
*       - Contains nodegraphs for completion
*       - Finish all targets in chain
*       - Node can also be a reward, given by system or by the npc
*
*/
// quest marker over an npc's head
#define quest_marker_available "?"  // has a quest to give
#define quest_marker_doing "+"      // you have its quest
#define quest_marker_done "!"       // you can hand its quest in
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "eve/_.c"
#include "tst/_.c"
#include "ui/_.c"

void import_quests(ecs* world) {
    zox_module(quests);
    zox_components_quests(world);
    zox_systems_quests(world);
    add_hook_spawn_prefabs(zox_prefabs_quests);
    add_hook_spawn_prefabs(zox_events_quests);
    zox_add_module(quests_ui);
}
