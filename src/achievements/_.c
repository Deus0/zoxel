#if !defined(zoxm_achievements) && defined(zoxm_users)
#define zoxm_achievements

// Realm Contains all Achievements
// Save/Load Players progress - per save game
// Toast UI -> show popup at top right when completed - this will trigger steam achievements at bottom right
// NOTE: Make sure to disable Steam Achievements while testing

#include "com/_.c"
#include "pre/_.c"
#include "fun/realm_achievements.c"
zox_declare_system_state_event(RealmAchievements, GenerateRealm, zox_generate_realm_achievements, spawn_realm_achievements);

zox_begin_module(Achievements)
    define_components_achievements(world);
    zox_define_system_state_event_1(RealmAchievements, EcsOnLoad, realms.GenerateRealm, [none] realms.Realm);
zox_end_module(Achievements)

#endif