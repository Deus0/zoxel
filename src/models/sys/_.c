#include "realm.c"
zox_declare_system_state_event(RealmModels, GenerateRealm, zox_generate_realm_models, spawn_realm_models)

void zox_define_systems_models(ecs* world) {
    zox_define_system_state_event_1(RealmModels, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm)
}