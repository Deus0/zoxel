zox_sys2(GamepadExtractSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SDLGamepad);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(SDLGamepad, gamepad);
        if (!sdl_gamepad_handle_disconnect(gamepad->value)) {
            continue;
        }
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, e, children, children_capacity);
        for (uint j = 0; j < children_length; j++) {
            entity e2 = children[j];
            sdl_extract_gamepad_button(world, e2, gamepad->value);
        }
    }
} zox_sys_end(GamepadExtractSystem);
