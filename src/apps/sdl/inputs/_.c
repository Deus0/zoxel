#ifndef zox_sdl_inputs
#define zox_sdl_inputs

byte using_sdl_gamecontrollers = 1;
int2 static_mouse_wheel;
#include "wrp/_.c"
#include "dat/_.c"
#include "com/_.c"
#include "pre/_.c"
#include "ins/_.c"
#include "fun/_.c"
#include "sys/_.c"
#include "dbg/_.c"

void update_sdl_inputs() {
    static_mouse_wheel = int2_zero;
}

void dispose_sdl_inputs(ecs *world, void *ctx) {
    close_sdl_input();
}

zox_begin_module(SdlInputs) {
    zox_module_dispose(dispose_sdl_inputs);
    zox_define_components_sdl_inputs(world);
    add_to_update_loop(update_sdl_inputs);
    add_hook_spawn_prefabs(spawn_prefabs_sdl_input);
    zox_define_systems_sdl_inputs(world);
    disable_virtual_keyboard();
    initialize_sdl_input();
    add_hook_on_boot(initialize_sdl_gamepads);
    // zox_logv("Virtual Keyboard Support? %i", can_virtual_keyboard());
} zox_end_module(SdlInputs);

#endif
