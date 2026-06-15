#ifndef zox_sdl_inputs
#define zox_sdl_inputs

// TODO: Support for SDL_GameController
byte using_sdl_gamecontrollers = 1;
#include "dat/settings.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

zox_begin_module(SdlInputs) {
    zox_define_components_sdl_inputs(world);
    zox_define_systems_sdl_inputs(world);
    initialize_sdl_input();
    add_hook_spawn_prefabs(spawn_prefabs_sdl_input);
    add_hook_on_boot(initialize_sdl_gamepads);
} zox_end_module(SdlInputs);

#endif
