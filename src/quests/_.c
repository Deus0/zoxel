#if !defined(zoxm_quests) && defined(zoxm_users)
#define zoxm_quests

#include "com/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "ui/_.c"

zox_begin_module(Quests)
    add_hook_spawn_prefabs(spawn_prefabs_quests);
    define_components_quests(world);
    define_systems_quests(world);
    zox_import_module(QuestsUI);
zox_end_module(Quests)

#endif
