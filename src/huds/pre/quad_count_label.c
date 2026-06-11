entity spawn_prefab_quad_count_label(ecs *world) {
    // const entity e = ecs_clone(world, 0, prefab_label_textured, 1);
    zox_prefab_child(prefab_label_textured)
    zox_add_tag(e, QuadsCountLabel)
    zox_prefab_set(e, QuadsCount, { -1 })
    prefab_set_mesh2D_vertices(world, e, square_vertices_right_aligned, 4);
    return e;
}

entity spawn_quad_count_label(ecs *world, entity canvas) {

    entity parent = canvas;
    byte font_size = 24;
    byte2 padding = (byte2) { 12, 8 };
    float2 anchor = { 1, 0 };
    int2 pixel_position = (int2) { -8, 8 };
    int layer = 1;
    int2 parent_pixel_size = zox_get_value(parent, LayoutSize)
    entity e = spawn_label(world, prefab_quad_count_label, parent, pixel_position, anchor, padding, "", font_size, zox_alignment_bottom_right, layer, button_fill, button_outline, default_label_font_fill_color, default_label_font_outline_color);

    return e;
}
