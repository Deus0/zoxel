void spawn_prefabs_sdl_input(ecs* world) {
    if (using_sdl_gamecontrollers) {
        zox_prefab_add(prefab_gamepad, SdlGameController);
    } else {
        zox_prefab_add(prefab_gamepad, SdlJoystick);
    }
}