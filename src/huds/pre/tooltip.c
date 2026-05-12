entity spawn_prefab_tooltip(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_add_tag(e, Tooltip);
    zox_add_tag(e, ZextRenderEnabler);
    zox_add_tag(e, MouseElement);
    zox_prefab_set(e, RenderDisabled, { 1 });
    zox_prefab_set(e, MeshAlignment, { zox_alignment_top_right });
    zox_prefab_set(e, ZeviceLink, { 0 });
    return e;
}

entity spawn_tooltip(ecs *world, entity prefab, entity canvas) {
    color tooltip_fill = color_white;
    color tooltip_fillo = color_black;
    color tooltip_font_fill = color_black; // (color) { 76, 66, 133, 255 };
    color tooltip_font_fillo = color_white; // (color) { 122, 122, 122, 255 };
    // Sizing
    byte font_size = 5 * ui_scale;
    byte2 padding = (byte2) { 3 * ui_scale, 2  * ui_scale };
    int2 position = (int2) { -2 * ui_scale, -2 * ui_scale };
    // Misc
    entity parent = canvas;
    int layer = max_layers2D - 2;
    byte alignment = zox_alignment_top_right;
    float2 anchor = float2_one;
    zox_geter_value(parent, LayoutSize, int2, parent_size);
    // le spawn
    entity e = spawn_label_background(world, prefab, parent, canvas, position, anchor, padding, "", font_size, alignment, layer, int2_half(parent_size), parent_size, tooltip_fill, tooltip_fillo, tooltip_font_fill,tooltip_font_fillo,  1);
    zox_name("tooltip");
    if (local_mouse) {
        uint children_capacity = zox_children_capacity;
        entity children[children_capacity];
        uint children_length = zox_get_children(world, local_mouse, children, children_capacity);
        if (children_length) {
            zox_set(e, ZeviceLink, { children[0] });
        } else {
            zox_loge("Mouse has no children.");
        }
    }
    return e;
}
