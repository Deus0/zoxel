
float main_menu_vignette = 2.2f;
float main_menu_blur = 0.4f;
float game_vignette = 1.0f;
float game_blur = 0;
float dialogue_vignette = 3.1f;
float dialogue_blur = 0.3f;
float pause_vignette = 2.2f;
#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(Cameras3) {
    add_hook_spawn_prefabs(spawn_prefabs_cameras3);
    define_systems_cameras3(world);
} zox_end_module(Cameras3);
