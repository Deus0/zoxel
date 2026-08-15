// NOTE: Vertical Scrollbar!
entity spawn_scrollbar(ecs *world, entity parent, int2 position, int2 size, int scrollbar_margins, int visible_elements, int total_elements, byte visible, entity scrollview) {
    // color fill = button_fill;
    // color outline = button_outline;
    float2 position_anchor = (float2) { 1.0f, 0.5f };
    // int2 size = (int2) { width, parent_size.y };
    int2 margins = (int2) { scrollbar_margins, 0 };
    int handle_height = !visible_elements ? size.y : (int) size.y * (float_min(1, (float) visible_elements / (float) total_elements));
    // top of scroll
    zox_instance(prefab_scrollbar);
    zox_name("scrollbar");
    zox_set_parent(world, e, parent);
    zox_set(e, LayoutPosition, { position });
    zox_set(e, LayoutSize, { size });
    zox_set(e, Anchor, { position_anchor });
    zox_set(e, BonusLayer, { 1 }); // increment it above the body of the window
    zox_set(e, FillColor, { button_fill });
    zox_set(e, OutlineColor, { button_outline });
    zox_set(e, ElementMargins, { margins });
    zox_set(e, RenderDisabled, { !visible });
    int2 handle_size = (int2) { size.x, handle_height };
    int2 handle_position = (int2) { 0, (size.y - handle_height) / 2 };
    entity handle = spawn_handle(world, prefab_handle, e, size, handle_position, handle_size, float2_half, handle_fill, handle_outline, 1);
    zox_add(handle, ScrollbarHandle);
    zox_set(handle, ScrollviewLink, { scrollview });
    zox_set(handle, RenderDisabled, { !visible });
    //zox_log("- scrollbar height set to: [%i] out of [%i]", height, parent_pixel_size.y)
    // zox_log("- scrollbar showing [%i] out of [%i]", visible_elements, total_elements);
    return e;
}
