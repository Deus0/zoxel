// #define zox_dbg_lines_unstuck

#include "fun/_.c"
#include "sys/_.c"
#include "eve/_.c"
#include "tst/_.c"

void import_terrain_collisions(ecs* world) {
    zox_module(terrain_collisions);
    zox_systems_terrain_collisions(world);
    add_hook_spawn_prefabs(zox_events_terrain_collisions);
    zox_tsts_terrain_collisions();
}
