entity spawn_element_basic(
    ecs *world,
    entity p,
    entity canvas,
    entity parent,
    int2 position,
    int2 pixel_size,
    int2 texture_size,
    float2 anchor,
    byte layer,
    int2 parent_position,
    int2 parent_size
) {

    zox_instance(p);
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
    entity p,
    entity canvas,
    int2 position,
    int2 pixel_size,
    int2 texture_size,
    float2 anchor
) {
    int2 canvas_size = zox_get_value(canvas, LayoutSize);

    return spawn_element_basic(
        world,
        p,
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

entity spawn_element_texture(ecs *world, entity canvas, entity t, int2 position, int2 size) {

    if (!t) {
        zox_log_error("[spawn_texture_element]: t is empty.")
        return 0;
    }

    zox_geter_value(t, TextureSize, int2, source_size);
    zox_geter(t, TextureData, source_data);

    entity e = spawn_element_basic_on_canvas(world,  prefab_element_shell, canvas, position, size, source_size, float2_zero);

    zox_set(e, TextureData, {
        .length = source_data->length,
        .value = source_data->value
    });
    zox_set(e, TextureDirty, { zox_dirty_trigger });

    zox_set(e, LayoutSizeDirty, { zox_dirty_trigger });

    return e;
}
