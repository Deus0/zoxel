entity2 spawn_elementbar2(ecs* world, entity prefab, entity parent, int2 position, int2 size, float2 position_anchor, byte font_size, byte2 text_padding, color back_color, color front_color) {
    color font_outline = (color) { 11, 11, 11, 222 };
    color font_fill = (color) { 225, 225, 225, 255 };
    // int2 text_size = (int2) { pixel_size.x - text_padding.x * 2, font_size };
    // Spawn our Element
    zox_instance(prefab);
    zox_name("elementbar2");
    zox_set_parent(world, e, parent);
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { position_anchor });
    zox_set(e, FillColor, { back_color });
    zox_set(e, OutlineColor, { color_black });
    // initialize_element(world, e, parent, canvas, pixel_position, pixel_size, int2_one, anchor, layer);
    byte padding = 6;
    zox_set(e, ElementBarSize, { (float2) { (size.x - padding * 2) / (float) size.x, 1 } });
    // frontbar
    float2 bar_anchor = (float2) { 0, 0.5f };
    entity e2 = spawn_uic(world, prefab_element_textured, e, bar_anchor, int2_zero, size, size, front_color, color_black);
    zox_set_unique_name(e2, "element2_frontbar");
    zox_set(e2, MeshAlignment, { zox_alignment_left });
    entity text = spawn_text(world, prefab_text, e, int2_zero, float2_half, font_size, zox_alignment_centre, text_padding, "", font_fill, font_outline);
    zox_set_unique_name(text, "element2_text");
    /*entity front_bar = spawn_elementbar2_front(
        world,
        canvas,
        e,
        int2_zero,
        pixel_size,
        front_bar_layer,
        render_disabled,
        front_color
    );
    zox_set_parent(world, front_bar, e);*/
    // text
    /*SpawnZext zextSpawnData = {
        .canvas = {
            .e = canvas
        },
        .parent = {
            .e = e,
            // .size = pixel_size
        },
        .element = {
            .prefab = prefab_text,
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
    entity text = spawn_text_old(world, zextSpawnData);
    zox_set_parent(world, text, e);*/
    return (entity2) { e, text };
}
