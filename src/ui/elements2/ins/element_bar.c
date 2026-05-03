entity2 spawn_elementbar2(
    ecs *world,
    const entity prefab,
    const entity canvas,
    const entity parent,
    int2 pixel_position,
    const int2 pixel_size,
    byte font_size,
    byte2 text_padding,
    float2 anchor,
    byte layer,
    int2 parent_pixel_positionv,
    int2 parent_pixel_size,
    byte render_disabled,
    color back_color,
    color front_color
) {
    // Misc
    const color label_font_outline_color = (color) { 33, 33, 33, 255 };
    const color label_font_fill_color = (color) { 155, 155, 155, 255 };
    const byte front_bar_layer = layer + 1;
    const byte zext_layer = layer + 2;
    // const byte text_length = 11; // health [100] - 11

    // Sizing characters
    // byte font_size = pixel_size.y - text_padding.y * 2;
    // const int zext_width = ;
    // const byte font_size_width = zext_width / text_length;
    /*if (font_size > font_size_width) {
        font_size = font_size_width;
    }*/
    const int2 text_size = (int2) { pixel_size.x - text_padding.x * 2, font_size };

    zox_instance(prefab);
    zox_name("elementbar2");
    zox_set(e, Color, { back_color });
    initialize_element(
        world,
        e,
        parent,
        canvas,
        pixel_position,
        pixel_size,
        int2_one,
        anchor,
        layer
    );
    if (render_disabled) {
        zox_set(e, RenderDisabled, { render_disabled });
    }
    const byte frontbar_padding = 6;
    zox_set(e, ElementBarSize, { (float2) { (pixel_size.x - frontbar_padding * 2) / (float) pixel_size.x, 1 } });

    Children children = (Children) { 0 };
    initialize_Children(&children, 2);

    // frontbar
    const entity front_bar = spawn_elementbar2_front(
        world,
        canvas,
        e,
        int2_zero,
        pixel_size,
        front_bar_layer,
        render_disabled,
        front_color
    );
    zox_set_unique_name(front_bar, "element2D_frontbar");
    children.value[0] = front_bar;

    // text
    SpawnZext zextSpawnData = {
        .canvas = {
            .e = canvas,
            // .size = canvas_size
        },
        .parent = {
            .e = e,
            .size = pixel_size
        },
        .element = {
            .prefab = prefab_zext,
            .layer = zext_layer,
            .anchor = float2_half,
            .render_disabled = render_disabled,
            .size = text_size
        },
        .zext = {
            .font_size = font_size,
            .font_resolution = font_size, //  default_label_font_resolution,
            .font_thickness = default_label_font_fill_thickness,
            .font_outline_thickness = default_label_font_outline_thickness,
            .font_fill_color = label_font_fill_color,
            .font_outline_color = label_font_outline_color
        }
    };

    entity text = spawn_zext(world, zextSpawnData);
    children.value[1] = text;
    zox_set_unique_name(text, "element2D_text");

    // finish
    zox_set_ptr(e, Children, children);

    return (entity2) { e, text };
}
