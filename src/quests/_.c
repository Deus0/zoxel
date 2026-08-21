/*
 * +------------------------------------------------------------------+
 * | Zox Module: Quests                                               |
 * |                                                                  |
 * |  Objectives - Progress - Rewards - Quest State - Chains          |
 * |                                                                  |
 * +------------------------------------------------------------------+
 *
*  TODO
*
*       - Quest Objective: Find NPC named Bob
*       - Contains nodegraphs for completion
*       - Finish all targets in chain
*       - Node can also be a reward, given by system or by the npc
*
*/
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "ui/_.c"

void import_quests(ecs* world) {
    zox_module(quests);
    define_components_quests(world);
    define_systems_quests(world);
    add_hook_spawn_prefabs(spawn_prefabs_quests);
    zox_import_module(QuestsUI);
}