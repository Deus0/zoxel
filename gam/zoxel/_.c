
byte mrpenguin_mode = 0;
// TODO: Generate Block + Model + Colors in Biomes themselves (single point of edit)
#include "fun/_.c"
#include "set/_.c"
#include "sys/_.c"

void import_zoxgame(ecs* world) {
    zox_module(zoxgame);
    game_name = "zoxel";
    define_systems_zoxel(world);
    add_hook_on_boot(spawn_weather);
    // Initial Zoxel Settings
    initialize_zoxel_settings(world);
    terrain_lod_near = 4;
    terrain_lod_far = 20;
    render_distance_y = 4;
    viewport_downscale = 1;
    zox_block_outlines = 1;
}
