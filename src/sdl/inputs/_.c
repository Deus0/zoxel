#ifndef zox_sdl_inputs
#define zox_sdl_inputs

#include "dat/settings.c"
#include "dat/sdl_gamepad.c"
#include "fun/_.c"
#include "sys/_.c"

void spawn_prefabs_sdl_input(ecs* world) {
#ifdef zox_sdl
    zox_prefab_add(prefab_gamepad, SDLGamepad);
#endif
}

zox_begin_module(SdlInputs)
#ifdef zox_sdl
    zox_define_component_w_dest(SDLGamepad);
    hide_virtual_keyboard();
#endif
    define_systems_sdl_inputs(world);
    initialize_sdl_input();
    add_hook_spawn_prefabs(spawn_prefabs_sdl_input);
zox_end_module(SdlInputs)

#endif