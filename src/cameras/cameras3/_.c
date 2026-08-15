#if !defined(zoxm_cameras3) && defined(zoxm_transforms3)
#define zoxm_cameras3

float main_menu_vignette = 3.1f;
float main_menu_blur = 0.4f;
float game_vignette = 1.2f;
float game_blur = 0;
float dialogue_vignette = 3.6f;
float dialogue_blur = 0.3f;
float pause_vignette = 2.6f;
#include "pre/_.c"
#include "sys/_.c"

zox_begin_module(Cameras3) {
    add_hook_spawn_prefabs(spawn_prefabs_cameras3);
    define_systems_cameras3(world);
} zox_end_module(Cameras3);

#endif
