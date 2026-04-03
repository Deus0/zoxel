entity spawn_element_basic(ecs *world, entity p, entity canvas, entity parent, int2 position, int2 pixel_size, int2 texture_size, float2 anchor, byte layer, int2 parent_position, int2 parent_size) {

    zox_instance(p);
    zox_name("element_basic");

    initialize_element(world, e, parent, canvas, position, pixel_size, texture_size, anchor, layer);

    return e;
}

entity spawn_element_texture(ecs *world, entity p, entity canvas, entity parent, int2 position, int2 size, float2 anchor, byte layer, entity t) {

    zox_instance(p);
    zox_name("element");

    zox_geter_value(t, TextureSize, int2, tsize);
    zox_geter(t, TextureData, source_data);

    initialize_element(world, e, parent, canvas, position, size, tsize, anchor, layer);

    zox_set_ptr(e, TextureData, *source_data);
    zox_set(e, TextureDirty, { zox_dirty_trigger });
    zox_set(e, LayoutSizeDirty, { zox_dirty_trigger });

    return e;
}


/*entity spawn_element_texture(ecs *world, entity p, entity canvas, entity t, int2 position, int2 size) {

    if (!t) {
        zox_log_error("[spawn_texture_element]: Invalid Texture")
        return 0;
    }

    zox_geter_value(t, TextureSize, int2, tsize);
    zox_geter(t, TextureData, source_data);

    int2 canvas_size = zox_get_value(canvas, LayoutSize);

    entity e = spawn_element_basic(world, p, canvas, canvas, position, tsize, tsize, float2_half, 0, int2_half(canvas_size), canvas_size);

    zox_set_ptr(e, TextureData, *source_data);
    zox_set(e, TextureDirty, { zox_dirty_trigger });
    zox_set(e, LayoutSizeDirty, { zox_dirty_trigger });

    zox_log("Spawned Texture UI len [%i]", source_data->length);

    return e;
}
*/

    /*zox_set(e, TextureData, {
        .length = source_data->length,
        .value = source_data->value
    });*/
