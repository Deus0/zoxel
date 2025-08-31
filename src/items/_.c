#if !defined(zoxm_items) && defined(zoxm_users)
#define zoxm_items

byte test_items_blocks = 0;
#include "com/_.c"
#include "pre/_.c"
#include "ui/_.c"
#include "fun/_.c"
#include "sys/_.c"

zox_begin_module(Items)
    define_components_items(world);
    define_systems_items(world);
    add_hook_spawned_character3D(&spawn_character_items);
    add_hook_spawn_prefabs(spawn_prefabs_items);
    zox_import_module(ItemsUI);
zox_end_module(Items)

#endif