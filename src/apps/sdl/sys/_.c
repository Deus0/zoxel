#include "hide_vk.c"

void define_systems_sdl(ecs* world) {
    zox_system(
        HideVirtualKeyboardSystem,
        EcsOnStore,
        [in] SDLWindow
    );
    zox_system(
        ToggleVirtualKeyboardSystem,
        EcsOnUpdate,
        [in] inputs.Keyboard
    );
}