
byte mrpenguin_mode = 0;
// TODO: Generate Block + Model + Colors in Biomes themselves (single point of edit)
#include "fun/_.c"
#include "set/_.c"
#include "sys/_.c"
#include "eve/_.c"

void import_zoxgame(ecs* world) {
    zox_module(zoxgame);
    game_name = "zoxel";
    define_systems_zoxel(world);
    add_hook_on_boot(spawn_weather);
    // Initial Zoxel Settings
    initialize_zoxel_settings(world);
    add_hook_spawn_prefabs(zox_events_zoxel);
}
