entity spawn_prefab_gamepad(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("gamepad");
    zox_add_tag(e, Gamepad);
    zox_prefab_set(e, DeviceLayout, { 0 });
    return e;
}

entity spawn_zevice_bumper(ecs* world, entity parent, const char* name, int rindex, int map) {
    entity e = zox_new();
    zox_set_unique_name(e, name);
    zox_set_parent(world, e, parent);
    zox_add_tag(e, Zevice);
    zox_prefab_set(e, ZeviceDisabled, { 0 });
    zox_prefab_set(e, DeviceButtonType, { map });
    zox_prefab_set(e, RealButtonIndex, { rindex });
    zox_add_tag(e, ZeviceBumper);
    zox_prefab_set(e, ZeviceButton, { 0 });
    return e;
}

entity spawn_gamepad(ecs *world, byte gamepad_type) {
    zox_instance(prefab_gamepad);
    zox_name("gamepad");
    zox_set(e, DeviceLayout, { gamepad_type });
    const byte *button_map = NULL;
    const byte *stick_map = NULL;
    const byte *dpad_map = NULL;
    switch (gamepad_type) {
        case zox_gamepad_layout_type_steamdeck:
            button_map = steamdeck_button_map;
            stick_map = steamdeck_stick_map;
            dpad_map = generic_dpad_map;
            break;
        default:
            button_map = gamepad_map_8bitdo;
            stick_map = generic_stick_map;
            dpad_map = generic_dpad_map;
            break;
    }
    // byte total = zox_gamepad_button_count + zox_gamepad_stick_count + zox_gamepad_dpad_count;
    byte i = 0;
    // Buttons
    for (byte j = 0; j < zox_gamepad_button_count; j++, i++) {
        entity e2 = spawn_device_button(world, prefab_zevice_button, i, button_map[j]);
        zox_set_parent(world, e2, e);
    }
    // Sticks
    for (byte j = 0; j < zox_gamepad_stick_count; j++, i++) {
        entity e2 = spawn_zevice_stick(world, e, j, stick_map[j]);
        zox_set_parent(world, e2, e);
    }
    // The DPAD
    for (byte j = 0; j < zox_gamepad_dpad_count; j++, i++) {
        entity e2 = spawn_device_button(world, prefab_zevice_button, i, dpad_map[j]);
        zox_set_parent(world, e2, e);
    }
    // todo: spawn LT and RT as axis for steamdeck
    {
        spawn_zevice_bumper(world, e, "left_trigger", 4, zox_device_button_lt);
    }
    {
        spawn_zevice_bumper(world, e, "right_trigger", 5, zox_device_button_rt);
    }
    gamepad_entity = e;
    return e;
}
