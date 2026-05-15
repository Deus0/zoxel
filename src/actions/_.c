#if !defined(zoxm_actions) && defined(zoxm_users) && defined(zoxm_items) && defined(zoxm_skills)
#define zoxm_actions

byte test_actions_blocks = 0;
byte test_actions_skills = 0;
// zoxc_userdata(Action);
zox_tag(Action);
zoxc_entity(ActionLink);
zoxc_entitiesw(ActionLinks);
zoxc_byte(ActionIndex);
#include "pre/_.c"
#include "fun/_.c"
#include "ui/_.c"
#include "sys/_.c"

zox_begin_module(Actions)
    zoxd_tag(Action);
    zoxd_entity(ActionLink);
    zoxd_entitiesw(ActionLinks);
    zoxd_byte(ActionIndex);
    add_hook_spawn_prefabs(spawn_prefabs_actions);
    define_systems_actions(world);
    // add_hook_spawned_character3D(&spawn_character_actions);
    zox_import_module(ActionsUI);
zox_end_module(Actions)

#endif
