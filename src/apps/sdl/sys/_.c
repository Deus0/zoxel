#include "hide_vk.c"

void define_systems_sdl(ecs* world) {
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
}