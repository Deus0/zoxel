/*
 * +------------------------------------------------------------------+
 * | Zox Module: Achievements                                         |
 * |                                                                  |
 * |  Goals - Progress - Unlocks - Rewards - Completion               |
 * |                                                                  |
 * +------------------------------------------------------------------+
 */
// Realm Contains all Achievements
// Save/Load Players progress - per save game
// Toast UI -> show popup at top right when completed - this will trigger steam achievements at bottom right
// NOTE: Make sure to disable Steam Achievements while testing

#include "com/_.c"
#include "pre/_.c"
#include "fun/realm_achievements.c"
#include "sys/_.c"

void import_achievements(ecs* world) {
    zox_module(achievements);
    zox_components_achievements(world);
    spawn_systems_achievements(world);
}
