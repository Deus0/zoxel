#include "realm.c"

zox_declare_system_state_event(RealmQuests, GenerateRealm, zox_generate_realm_quests, spawn_realm_quests)

void define_systems_quests(ecs* world) {
    zox_define_system_state_event_1(RealmQuests, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
}