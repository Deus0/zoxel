entity spawn_layout2(ecs *world, entity p, entity canvas, entity parent, int2 position, int2 size, float2 anchor, byte layer, int2 parent_position, int2 parent_size) {
    zox_instance(p);
    zox_name("layout2");
    initialize_layout2(world, e, parent, canvas, position, size, anchor, layer);
    return e;
}

entity spawn_layout2_on_canvas(ecs *world, entity p, entity canvas, int2 pixel_position, int2 pixel_size, float2 anchor) {
    int2 canvas_size = zox_get_value(canvas, LayoutSize);
    return spawn_layout2(world, p, canvas, canvas, pixel_position, pixel_size, anchor, 0, int2_half(canvas_size), canvas_size);
}
