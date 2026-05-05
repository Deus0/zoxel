entity spawn_element(ecs *world, ElementSpawn data) {
    entity prefab = data.element.prefab;
    entity parent = data.parent.e;
    entity canvas = data.canvas.e;
    int2 position = data.element.position;
    int2 size = data.element.size;
    float2 anchor = data.element.anchor;
    byte layer = data.element.layer;
    int2 tsize = data.element.texture_size;
    color fill = data.texture.fill_color;
    color outline = data.texture.outline_color;
    zox_instance(prefab);
    zox_name("element");
    zox_set_parent(world, e, parent);
    // zox_set(e, ParentLink, { parent });
    zox_set(e, CanvasLink, { canvas });
    zox_set(e, Layer2D, { layer });
    zox_set(e, Anchor, { anchor });
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, TextureSize, { tsize });
    zox_set(e, Color, { fill });
    zox_set(e, OutlineColor, { outline });
    // Where we link to canvas children
    if (canvas == parent) {
        zox_set(canvas, WindowToTop, { e });
    }
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

    return spawn_element(world, spawn_element_data);
}

entity spawn_element2(ecs* world, entity prefab, entity canvas, entity parent, int2 position, int2 size, int2 tsize, float2 anchor, byte layer, color fcolor, color ocolor) {

    zox_instance(prefab);
    zox_name("element");

    initialize_element(world, e, parent, canvas, position, size, tsize, anchor, layer);

    zox_set(e, Color, { fcolor });
    zox_set(e, OutlineColor, { ocolor });

    return e;
}

entity spawn_element3(ecs *world, entity prefab, float2 anchor, int2 position, int2 size, int2 texture_size, color fill, color outline) {

    zox_instance(prefab);
    zox_name("element");
    zox_set(e, Anchor, { anchor });
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, TextureSize, { texture_size });
    zox_set(e, Color, { fill });
    zox_set(e, OutlineColor, { outline });

    // zox_set(e, CanvasLink, { canvas });

    // Where we link to canvas children
    // zox_set(e, ParentLink, { parent });
    // if (canvas == parent) zox_set(canvas, WindowToTop, { e });
    // zox_set(e, Layer2D, { layer });

    return e;
}
