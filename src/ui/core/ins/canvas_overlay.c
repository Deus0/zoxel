// todo: stretch to parent size!
entity spawn_canvas_overlay(
    ecs *world,
    const entity prefab,
    const entity canvas,
    const int2 canvas_size
) {
    const entity parent = canvas;
    const byte layer = game_overlay_layer;
    const int2 pixel_position = int2_zero;
    const float2 anchor = float2_half;
    const int2 pixel_size = (int2) { 4096, 4096 }; //  canvas_size;

    zox_instance(prefab)
    zox_name("canvas_overlay")
    initialize_element(
        world,
        e,
        parent,
        canvas,
        pixel_position,
        pixel_size,
        pixel_size,
        anchor,
        layer,
        float2_zero, // position2,
        int2_zero // pixel_positionv
    );

    // on_child_added(world, canvas, e);
    zox_set(e, Alpha, { 1 });
    trigger_canvas_overlay_fade_out(world, e, 1.5f, 1.5f);

    return e;
}
