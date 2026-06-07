/*
*  Quests
*
*       - Contains nodegraphs for completion
*       - Finish all targets in chain
*       - Node can also be a reward, given by system or by the npc on return
*
*/
#if !defined(zoxm_quests) && defined(zoxm_users)
#define zoxm_quests

// TODO: Quest Objective: Find NPC named Bob

#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "ui/_.c"

zox_begin_module(Quests) {
    define_components_quests(world);
    define_systems_quests(world);
    add_hook_spawn_prefabs(spawn_prefabs_quests);
    zox_import_module(QuestsUI);
} zox_end_module(Quests);

#endif
