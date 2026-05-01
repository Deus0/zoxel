zox_sys2(GamepadExtractSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(Children);
    zox_sys_in(SDLGamepad);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(Children, children);
        zox_sys_i(SDLGamepad, gamepad);

        if (sdl_gamepad_handle_disconnect(gamepad->value)) {
            sdl_extract_gamepad(gamepad->value, world, children);
        }
    }
} zox_sys_end(GamepadExtractSystem);