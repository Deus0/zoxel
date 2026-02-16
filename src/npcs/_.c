#if !defined(zoxm_ai) && defined(zoxm_characters)
#define zoxm_ai

// TODO: a threat level - if you are higher level than npc they should have higher chance to flee!

#include "com/_.c"
#include "sta/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"

void set_disable_npcs(ecs* world, void* value) {
    (void) world;
    disable_npcs = *(byte*) value;
}

void initialize_settings_npcs(ecs *world) {
    zoxs_new_byte("disable npcs", set_disable_npcs, 0)
}

zox_begin_module(Npcs)
    define_components_npcs(world);
    define_systems_npcs(world);
    add_hook_spawn_prefabs(spawn_prefabs_npcs);
    initialize_settings_npcs(world);
zox_end_module(Npcs)

#endif
