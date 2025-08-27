#ifndef zoxm_users
#define zoxm_users

#include "com/_.c"
#include "util/user_data.c"
#include "ui/_.c"
zox_increment_system_with_reset(Activate, zox_dirty_end);

zox_begin_module(Users)
    define_components_users(world);
    zoxd_system_increment(Activate);
    zox_import_module(UsersUI);
zox_end_module(Users)

#endif
