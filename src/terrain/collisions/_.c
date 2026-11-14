// #define zox_dbg_lines_unstuck

#include "fun/_.c"
#include "sys/_.c"
#include "eve/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_terrain_collisions(ecs* world) {
    zox_module(terrain_collisions);
    zox_systems_terrain_collisions(world);
    add_hook_spawn_prefabs(zox_events_terrain_collisions);
#ifdef zox_tests
    zox_tsts_terrain_collisions();
#endif
}
