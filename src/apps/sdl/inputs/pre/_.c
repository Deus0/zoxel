void spawn_prefabs_sdl_input(ecs* world) {
    if (using_sdl_gamecontrollers) {
        zox_add(prefab_gamepad, SdlGameController);
    } else {
        zox_add(prefab_gamepad, SdlJoystick);
    }
}