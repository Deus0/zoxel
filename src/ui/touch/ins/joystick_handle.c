entity spawn_virtual_joystick_pointer(
    ecs *world,
    const entity canvas,
    const entity parent,
    const byte layer,
    const int2 pixel_position,
    const float2 anchor,
    const int2 parent_pixel_positionv,
    const int2 parent_pixel_size,
    const int2 canvas_size
) {
    int2 pixel_size = virtual_joystick_pointer_size;

    zox_instance(prefab_virtual_joystick_pointer)
    zox_name("virtual_joystick_pointer")
    initialize_element(
        world,
        e,
        parent,
        canvas,
        pixel_position,
        pixel_size,
        pixel_size,
        anchor,
        layer
    );
    return e;
}
