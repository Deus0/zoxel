extern entity dbg_player;

entity spawn_gamepad_sdl_controller(ecs* world, entity app, zox_sdl_gamepad* controller) {
    //entity game = zox_get_child_by_id(world, parent, zox_id(Game));
    //entity player = zox_get_child_by_id(world, game, zox_id(Player));
    // entity game = zox_get_child_by_index(world, app, 0);
    // entity player = zox_get_child_by_index(world, game, 0);
    const char* name = zox_sdl_gamepad_name(controller);
    entity e = spawn_gamepad_new(world, app);
    zox_name(name);
    // zox_set_parent(world, e, player);
    zox_set(e, SdlGameController, { controller });
    zox_log("New Gamepad [%s] added to [%s]", name, zox_get_name(app));
    // zox_set_parent(world, e, parent);
    return e;
}

/*byte is_steamdeck_gamepad(SDL_Joystick *gamepad) {
    if (!gamepad) {
        return 0;
    }
    return zox_sdl_joystick_vendor(gamepad) == 0x28DE;
}

byte is_xbox_gamepad(SDL_Joystick *gamepad) {
    if (!gamepad) {
        return 0;
    }
    Uint16 vendor = zox_sdl_joystick_vendor(gamepad);
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

entity spawn_gamepad_sdl_joystick(ecs* world, entity parent, SDL_Joystick* joystick) {
    byte gamepad_type = get_gamepad_type(joystick);
    entity e = spawn_gamepad(world, gamepad_type);
    zox_set_parent(world, e, parent);
    // zox_set_parent(world, e, dbg_player);
    zox_set(e, SdlJoystick, { joystick });
    // zox_logv("   + gamepad [%s] on parent [%s]", SDL_JoystickName(joystick), zox_get_name(parent));
    zox_logv("   + gamepad on [%s]", zox_get_name(parent));
    return e;
}*/