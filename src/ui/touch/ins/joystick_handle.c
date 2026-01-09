entity spawn_virtual_joystick_handle(
    ecs *world,
    const entity canvas,
    const entity parent,
    const byte layer,
    const int2 position,
    const int2 size,
    const float2 anchor
) {
    zox_instance(prefab_virtual_joystick_pointer);
    zox_name("virtual_joystick_pointer");
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
    return e;
}
