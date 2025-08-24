entity spawn_layout2(ecs *world,
    const entity prefab,
    const entity canvas,
    const entity parent,
    int2 position,
    const int2 size,
    const float2 anchor,
    const byte layer,
    const int2 parent_position,
    const int2 parent_size
) {
    zox_instance(prefab)
    zox_name("layout2")
    initialize_element_invisible(world,
        e,
        parent,
        canvas,
        position,
        size,
        anchor,
        layer,
        float2_zero,
        int2_zero
    );
    return e;
}

entity spawn_layout2_on_canvas(ecs *world,
    const entity prefab,
    const entity canvas,
    const int2 pixel_position,
    const int2 pixel_size,
    const float2 anchor)
{
    const int2 canvas_size = zox_get_value(canvas, LayoutSize)
    return spawn_layout2(world, prefab, canvas, canvas, pixel_position, pixel_size, anchor, 0, int2_half(canvas_size), canvas_size);
}
