#if !defined(zoxm_ai) && defined(zoxm_characters)
#define zoxm_ai

float default_npc_follow_distance = 0.45f;
// TODO: a threat level - if you are higher level than npc they should have higher chance to flee!
#include "com/_.c"
#include "sta/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "fun/_.c"

zox_begin_module(Npcs) {
    define_components_npcs(world);
    define_systems_npcs(world);
    add_hook_spawn_prefabs(spawn_prefabs_npcs);
} zox_end_module(Npcs);

#endif
