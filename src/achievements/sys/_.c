#include "realm.c"
zox_declare_system_state_event(RealmAchievements, GenerateRealm, zox_generate_realm_achievements, spawn_realm_achievements);

void spawn_systems_achievements(ecs* world) {
    zox_define_system_state_event_1(RealmAchievements, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
}
