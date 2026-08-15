entity2 spawn_bar2(ecs* world, entity prefab, entity parent, int2 position, int2 size, float2 position_anchor, byte font_size, byte2 text_padding, color back_color, color front_color) {
    entity2 output;
    color font_outline = (color) { 11, 11, 11, 222 };
    color font_fill = (color) { 225, 225, 225, 255 };
    zox_instance(prefab);
    output.x = e;
    zox_name("bar2");
    zox_set_parent(world, e, parent);
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { position_anchor });
    zox_set(e, FillColor, { back_color });
    zox_set(e, OutlineColor, { color_black });
    byte padding = 6;
    // zox_set(e, ElementBarSize, { (float2) { (size.x - padding * 2) / (float) size.x, 1 } });
    // frontbar
    {
        float2 bar_anchor = float2_centre;
        entity e2 = spawn_uic(
            world,
            prefab_element_frame,
            e,
            bar_anchor,
            int2_zero,
            size,
            size,
            front_color,
            color_black);
        zox_set_unique_name(e2, "bar2_frontbar");
        zox_add(e2, BarFront);
        // zox_set(e2, MeshAlignment, { zox_alignment_left });
    }
    {
        entity e2 = spawn_text(world, prefab_text, e, int2_zero, float2_half, font_size, zox_alignment_centre, text_padding, "", font_fill, font_outline);
        zox_set_unique_name(e2, "bar2_text");
        output.y = e2;
    }
    return  output;
}
