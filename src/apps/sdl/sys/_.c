#include "hide_vk.c"
#include "events.c"

void zox_systems_sdl(ecs* world) {
    zox_system_1(
        sdl_events_system,
        zoxp_begin,
        0 // [none] SDLWindow
    );
    zox_system(
        HideVirtualKeyboardSystem,
        zoxp_update,
        [in] SDLWindow
    );
    zox_system(
        ToggleVirtualKeyboardSystem,
        zoxp_update,
        [in] inputs.Keyboard
    );
    zox_system_1(
        app_gpu_end_system,
        zoxp_render_end,
        0
    );
}