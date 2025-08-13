

// todo: stretch to parent size!
entity spawn_canvas_overlay(ecs *world,
    const entity prefab,
    const entity canvas,
    const int2 canvas_size)
{
    #ifdef zox_disable_canvas_overlay
    return 0;
    #endif
    const entity parent = canvas;
    const byte layer = game_overlay_layer;
    const int2 pixel_position = int2_zero;
    const float2 anchor = float2_half;
    const int2 pixel_size = (int2) { 4096, 4096 }; //  canvas_size;
    const int2 pixel_positionv = get_element_pixel_positionv(int2_half(canvas_size), canvas_size, pixel_position, anchor);
    const float2 position2D = get_element_position(pixel_positionv, canvas_size);
    zox_instance(prefab)
    zox_name("canvas_overlay")
    initialize_element(world, e, parent, canvas, pixel_position, pixel_size, pixel_size, anchor, layer, position2D, pixel_positionv);
    // on_child_added(world, canvas, e);
    zox_set(e, Alpha, { 1 })
    trigger_canvas_fade_out(world, e);
    return e;
}
