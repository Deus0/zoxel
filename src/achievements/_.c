#if !defined(zoxm_achievements) && defined(zoxm_users)
#define zoxm_achievements

// Realm Contains all Achievements
// Save/Load Players progress - per save game
// Toast UI -> show popup at top right when completed - this will trigger steam achievements at bottom right
// NOTE: Make sure to disable Steam Achievements while testing

#include "com/_.c"
#include "pre/_.c"
#include "fun/realm_achievements.c"
#include "sys/_.c"

zox_begin_module(Achievements)
    define_components_achievements(world);
    spawn_systems_achievements(world);
zox_end_module(Achievements)

#endif
