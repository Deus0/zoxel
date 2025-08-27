#ifndef zoxm_users
#define zoxm_users

zoxc_byte(Activate);
zoxc_byte(Quantity);
zoxc_entity(UserLink);
zoxc_entity(UserDataLink);
zoxc_entity(SpawnerLink);
zoxc_entities(UserLinks)
#include "util/user_data.c"
#include "ui/_.c"
zox_increment_system_with_reset(Activate, zox_dirty_end);

zox_begin_module(Users)
    zoxd_byte(Activate);
    zoxd_byte(Quantity);
    zoxd_entity(UserLink);
    zoxd_entity(UserDataLink);
    zoxd_entity(SpawnerLink);
    zox_define_entities_component(UserLinks);
    zoxd_system_increment(Activate);
    zox_import_module(UsersUI);
zox_end_module(Users)

#endif
