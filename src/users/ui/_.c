#ifndef zoxm_users_ui
#define zoxm_users_ui

zox_tag(UserIcon);
zox_tag(UserIconWindow);
zox_tag(MenuStatus);
#include "dat/_.c"
#include "pre/_.c"
#include "fun/_.c"
#include "ins/_.c"
#include "sys/_.c"
#include "fun/menu_macro.c"

zox_begin_module(UsersUI)
    add_hook_spawn_prefabs(spawn_prefabs_users_ui);
    zoxd_tag(UserIcon);
    zoxd_tag(UserIconWindow);
    zoxd_tag(MenuStatus);
    define_systems_users_ui(world);
zox_end_module(Users)

#endif
