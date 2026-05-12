#ifndef zoxm_users
#define zoxm_users

byte zox_log_activations = 1;
#include "com/_.c"
#include "pre/_.c"
#include "sys/_.c"
#include "fun/_.c"
#include "ui/_.c"

zox_begin_module(Users)
    define_components_users(world);
    define_systems_users(world);
    add_hook_spawn_prefabs(spawn_prefabs_users);
    zox_import_module(UsersUI);
zox_end_module(Users)

#endif