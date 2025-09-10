#include "character.c"
#include "realm.c"
zox_declare_system_state_event(RealmEquips, GenerateRealm, zox_generate_realm_equips, spawn_realm_equips)

void define_systems_equipment(ecs* world) {
    zox_define_system_state_event_1(RealmEquips, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
}