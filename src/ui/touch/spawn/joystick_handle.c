entity spawn_virtual_joystick_pointer(ecs *world,
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
    const int2 pixel_positionv = get_element_pixel_positionv(parent_pixel_positionv, parent_pixel_size, pixel_position, anchor);
    const float2 position2 = get_element_position(pixel_positionv, canvas_size);
    zox_instance(prefab_virtual_joystick_pointer)
    zox_name("virtual_joystick_pointer")
    initialize_element(world, e, parent, canvas, pixel_position, pixel_size, pixel_size, anchor, layer, position2, pixel_positionv);
    return e;
}
