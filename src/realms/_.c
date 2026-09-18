
// todo: we really should fadeoout first before generating realm!
#include "dat/generate_realm.c"
#include "com/_.c"
#include "mcr/_.c"
#include "pre/_.c"
#include "dbg/_.c"
zox_increment_system_with_reset(GenerateRealm, zox_generate_realm_end);

void import_realms(ecs* world) {
    zox_module(realms);
    zox_components_realms(world);
    zoxd_system_increment(
        GenerateRealm,
        [none] Realm
    );
    add_hook_spawn_prefabs(spawn_prefabs_realms);
}