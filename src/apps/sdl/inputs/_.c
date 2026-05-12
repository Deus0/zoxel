#ifndef zox_sdl_inputs
#define zox_sdl_inputs

#include "dat/settings.c"
#include "dat/sdl_gamepad.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void spawn_prefabs_sdl_input(ecs* world) {
    zox_prefab_add(prefab_gamepad, SDLGamepad);
}

zox_begin_module(SdlInputs)
    zoxd_dest(SDLGamepad);
    define_systems_sdl_inputs(world);
    initialize_sdl_input();
    add_hook_spawn_prefabs(spawn_prefabs_sdl_input);
    add_hook_on_boot(initialize_sdl_gamepads);
zox_end_module(SdlInputs)

#endif
