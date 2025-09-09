#ifndef zoxm_realms
#define zoxm_realms

// todo: we really should fadeoout first before generating realm!
#include "dat/generate_realm.c"
zox_tag(Realm);
zoxc_byte(GenerateRealm);
zoxc_entity(RealmLink);
#include "pre/_.c"
zox_increment_system_with_reset(GenerateRealm, zox_generate_realm_end);

zox_begin_module(Realms)
    zoxd_tag(Realm);
    zoxd_byte(GenerateRealm);
    zoxd_entity(RealmLink);
    zoxd_system_increment(GenerateRealm, [none] Realm);
    add_hook_spawn_prefabs(spawn_prefabs_realms);
zox_end_module(Realms)

#endif