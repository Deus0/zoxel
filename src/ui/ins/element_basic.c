entity spawn_element_basic(
    ecs *world,
    const entity prefab,
    const entity canvas,
    const entity parent,
    int2 position,
    const int2 pixel_size,
    const int2 texture_size,
    const float2 anchor,
    const byte layer,
    const int2 parent_position,
    const int2 parent_size
) {

    zox_instance(prefab);
    zox_name("element_basic");
    initialize_element(
        world,
        e,
        parent,
        canvas,
        position,
        pixel_size,
        texture_size,
        anchor,
        layer
    );

    return e;
}

entity spawn_element_basic_on_canvas(
    ecs *world,
    const entity canvas,
    const int2 position,
    const int2 pixel_size,
    const int2 texture_size,
    const float2 anchor
) {
    const int2 canvas_size = zox_get_value(canvas, LayoutSize)
    return spawn_element_basic(
        world,
        prefab_element_shell,
        canvas,
        canvas,
        position,
        pixel_size,
        texture_size,
        anchor,
        0,
        int2_half(canvas_size),
        canvas_size);
}

entity spawn_element_texture(
    ecs *world,
    const entity canvas,
    const entity source_texture,
    const int2 position,
    const int2 size
) {
    if (!source_texture) {
        zox_log_error("[spawn_texture_element]: source_texture is empty.")
        return 0;
    }

    zox_geter_value(source_texture, TextureSize, int2, source_size);
    zox_geter(source_texture, TextureData, source_data);

    const entity e = spawn_element_basic_on_canvas(
        world,
        canvas,
        position,
        size,
        source_size,
        float2_zero
    );

    zox_set(e, TextureData, {
        .length = source_data->length,
        .value = source_data->value
    });
    zox_set(e, TextureDirty, { 1 });

    return e;
}
