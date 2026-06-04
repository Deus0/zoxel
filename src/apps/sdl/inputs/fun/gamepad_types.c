byte is_steamdeck_gamepad(SDL_Joystick *gamepad) {
    if (!gamepad) {
        return 0;
    }
    return SDL_JoystickGetVendor(gamepad) == 0x28DE;
}

byte is_xbox_gamepad(SDL_Joystick *gamepad) {
    if (!gamepad) {
        return 0;
    }
    Uint16 vendor = SDL_JoystickGetVendor(gamepad);
    switch (vendor) {
        case 0x045E: // Microsoft
        case 0x0738: // Mad Catz
        case 0x0E6F: // PDP
        case 0x1532: // Razer
        case 0x24C6: // PowerA
            return 1;
        default:
            return 0;
    }
}

byte get_gamepad_type(SDL_Joystick *joystick) {
    byte gamepad_type = zox_gamepad_layout_type_xbox; // default
    if (is_xbox_gamepad(joystick)) {
        gamepad_type = zox_gamepad_layout_type_xbox;
    }
    if (is_steamdeck_gamepad(joystick)) {
        gamepad_type = zox_gamepad_layout_type_steamdeck;
    }
    return gamepad_type;
}
