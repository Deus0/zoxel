entity spawn_virtual_joystick(
    ecs *world,
    const entity canvas,
    const int2 position,
    const entity finger,
    const entity virtual_joystick,
    const byte button_type
) {
    // Sizing
    int2 size = int2_single((zox_virtual_joystick_size / 4) * ui_scale);
    int2 pointer_size = int2_single((zox_virtual_joystick_pointer_size / 4) * ui_scale);

    // Misc
    const float2 anchor = float2_zero;
    const byte layer = 4;

    zox_instance(prefab_virtual_joystick);
    zox_name("virtual_joystick");
    entity parent = canvas;
    initialize_element(
        world,
        e,
        parent,
        canvas,
        position,
        size,
        size,
        anchor,
        layer
    );

    Children children = (Children) { 0 };
    entity joystick = spawn_virtual_joystick_handle(
        world,
        canvas,
        e,
        (layer + 1),
        int2_zero,
        pointer_size,
        float2_half
    );
    add_to_Children(&children, joystick);
    zox_set_ptr(e, Children, children);

    // links
    zox_set(e, ZeviceLink, { virtual_joystick });
    zox_set(virtual_joystick, ElementLink, { e });
    zox_set(finger, DeviceButtonType, { button_type });
    zox_set(virtual_joystick, DeviceButtonType, { button_type });

    if (button_type == zox_device_stick_right) {
        zox_set(e, Color, { virtual_joystick_color2 });
        zox_set(joystick, Color, { virtual_joystick_pointer_color2 });
    }

    return e;
}
