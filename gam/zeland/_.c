// Game Directory
// zeland: walks a poly terrain (doc/plans/poly_terrains.md), no voxels; for now plays with a free camera in the void
#include "eve/_.c"
#ifdef zox_tests
#include "tst/_.c"
#endif

void import_zoxgame(ecs* world) {
    zox_module(zoxgame);
    game_name = "zeland";
    zox_game_type = zox_game_mode_poly;
    menu_sky_color = (color_rgb) { 120, 170, 220 };
    menu_sky_bottom_color = (color_rgb) { 200, 220, 235 };
    game_sky_color = (color_rgb) { 110, 165, 225 };
    game_sky_bottom_color = (color_rgb) { 215, 230, 240 };
    add_hook_on_boot(spawn_weather);
    zox_events_zeland(world);
#ifdef zox_tests
    zox_tsts_zeland(world);
#endif
}
