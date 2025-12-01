// todo: stretch to canvas_size, AnchorSize
entity spawn_render_texture(
    ecs *world,
    const entity prefab,
    const entity canvas,
    const int2 layout_size,
    const int2 texture_size,
    const entity camera
) {
    const byte layer = 0;
    const entity parent = canvas;
    const int2 position = int2_zero;
    const float2 anchor = float2_half;

    zox_instance(prefab)
    zox_name("render_texture")
    zox_set(e, CameraLink, { camera });

    zox_log("render texture s %ix%i ts %ix%i", layout_size.x, layout_size.y, texture_size.x, texture_size.y);

    initialize_element(
        world,
        e,
        parent,
        canvas,
        position,
        layout_size,
        texture_size,
        anchor,
        layer
    );

    return e;
}
