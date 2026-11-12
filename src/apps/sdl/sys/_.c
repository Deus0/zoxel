#include "hide_vk.c"
#include "events.c"

void zox_systems_sdl(ecs* world) {
    zox_system_1(
        sdl_events_system,
        zoxp_begin,
        0 // [none] SDLWindow
    );
    zox_system(
        hide_virtual_keyboard_system,
        zoxp_update,
        [in] SDLWindow
    );
    zox_system(
        toggle_virtual_keyboard_system,
        zoxp_update,
        [in] inputs.Keyboard
    );
    zox_system_1(
        app_gpu_end_system,
        zoxp_render_end,
        0
    );
}