entity spawn_virtual_joystick(ecs *world, entity canvas, int2 position, entity finger, entity virtual_joystick, byte button_type) {
    // Sizing
    int2 size = int2_single((zox_virtual_joystick_size / 4) * ui_scale);
    int2 pointer_size = int2_single((zox_virtual_joystick_pointer_size / 4) * ui_scale);
    // Misc
    float2 anchor = float2_zero;
    byte layer = 4;
    zox_instance(prefab_virtual_joystick);
    zox_name("virtual_joystick");
    entity parent = canvas;
    initialize_element(world, e, parent, canvas, position, size, size, anchor, layer);
    entity e2 = spawn_ui(world, prefab_element_frame, e, float2_half, int2_zero, pointer_size, pointer_size);
    zox_set(e2, Layer2D, { (layer + 1) });
    // links
    zox_set(e, ZeviceLink, { virtual_joystick });
    zox_set(virtual_joystick, ElementLink, { e });
    zox_set(virtual_joystick, DeviceButtonType, { button_type });
    if (button_type == zox_device_stick_right) {
        zox_set(e, FillColor, { virtual_joystick_color2 });
        zox_set(e2, FillColor, { virtual_joystick_pointer_color2 });
    }
    return e;
}
