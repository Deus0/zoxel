#ifndef zoxm_characters2
#define zoxm_characters2

int spawn_characters2D_count = 64;
int2 character2DTextureSize = { 16, 16 };

zox_tag(Character2D);
zoxc_entity(Character2DLink);

#include "pre/_.c"
#include "ins/_.c"
#include "fun/spawn_many.c"
#include "sys/bob_spawn_system.c"

zox_begin_module(Characters2)
    zoxd_tag(Character2D);
    zoxd_entity(Character2DLink);
    add_hook_spawn_prefabs(spawn_prefabs_characters2);
zox_end_module(Characters2)

#endif
