#ifndef zoxm_users
#define zoxm_users

#include "com/_.c"
#include "sys/_.c"
#include "fun/user_data.c"
#include "ui/_.c"

zox_begin_module(Users)
    define_components_users(world);
    define_systems_users(world);
    zox_import_module(UsersUI);
zox_end_module(Users)

#endif