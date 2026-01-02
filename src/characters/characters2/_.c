#ifndef zoxm_characters2
#define zoxm_characters2

const int spawn_characters2D_count = 64;
const int2 character2DTextureSize = { 16, 16 };
entity prefab_character2D;
zox_tag(Character2D);
zoxc_entity(Character2DLink);
#include "pre/character2D.c"
#include "fun/spawn_many.c"
#include "sys/bob_spawn_system.c"

void spawn_prefabs_characters2D(ecs *world) {
    prefab_character2D = spawn_prefab_character2(world);
}

zox_begin_module(Characters2)
    zoxd_tag(Character2D);
    zoxd_entity(Character2DLink);
    add_hook_spawn_prefabs(spawn_prefabs_characters2D);
zox_end_module(Characters2)

#endif
