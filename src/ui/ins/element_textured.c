entity spawn_element(ecs *world, const ElementSpawn *data) {

    zox_instance(data->element.prefab);
    zox_name("element");

    initialize_element(world, e, data->parent.e, data->canvas.e, data->element.position, data->element.size, data->element.size, data->element.anchor, data->element.layer);

    zox_set(e, Color, { data->texture.fill_color });
    zox_set(e, OutlineColor, { data->texture.outline_color });

    return e;
}

entity spawn_element_on_canvas(ecs *world, entity canvas, int2 position, int2 size, float2 anchor) {

    int2 canvas_size = zox_get_value(canvas, LayoutSize);

    ElementSpawn spawn_element_data = {
        .canvas = {
            .e = canvas,
            .size = canvas_size
        },
        .parent = {
            .e = canvas,
            .position = int2_half(canvas_size),
            .size = canvas_size
        },
        .element = {
            .prefab = prefab_element_textured,
            .layer = 2,
            .anchor = anchor,
            .position = position,
            .size = size
        },
        .texture = {
            .fill_color = default_fill_color,
            .outline_color = default_outline_color,
        }
    };

    return spawn_element(world, &spawn_element_data);
}

entity spawn_element2(ecs *world, entity p, entity canvas, entity parent, int2 position, int2 size, int2 tsize, float2 anchor, byte layer, color fcolor, color ocolor) {

    zox_instance(p);
    zox_name("element");

    initialize_element(world, e, parent, canvas, position, size, tsize, anchor, layer);

    zox_set(e, Color, { fcolor });
    zox_set(e, OutlineColor, { ocolor });

    return e;
}
