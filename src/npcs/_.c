#if !defined(zoxm_ai) && defined(zoxm_characters)
#define zoxm_ai

// TODO: a threat level - if you are higher level than npc they should have higher chance to flee!

#include "com/_.c"
#include "sta/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"

zox_begin_module(Npcs)
    define_components_npcs(world);
    define_systems_npcs(world);
    add_hook_spawn_prefabs(spawn_prefabs_npcs);
zox_end_module(Npcs)

#endif
