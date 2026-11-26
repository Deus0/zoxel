entity spawn_scrollbar(
    ecs *world,
    entity parent,
    int2 position,
    int2 size,
    int scrollbar_margins,
    int visible_elements,
    int total_elements,
    byte visible,
    entity scrollview,
    byte horizontal)
{
    const float2 position_anchor = horizontal ? (float2) { 0.5f, 0 } : (float2) { 1, 0.5f };
    int2 margins = (int2) { scrollbar_margins, 0 };
    int handle_height = !visible_elements ? size.y : (int) size.y * (float_min(1, (float) visible_elements / (float) total_elements));
    const int2 handle_size = horizontal ? size : (int2) { size.x, handle_height };
    const int2 handle_position = horizontal ? int2_zero : (int2) { 0, (size.y - handle_height) / 2 };
    // top of scroll
    zox_instance(ui_prefab_for(world, prefab_scrollbar, parent));    // its 3D twin under a 3D parent
    zox_name(horizontal ? "horizontal_scrollbar" : "scrollbar");
    if (horizontal) {
        zox_add(e, HorizontalScrollbar);
    } else {
        zox_add(e, Scrollbar);
    }
    zox_set_parent_non_fragment(world, e, parent);
    ui_layer3_set(world, e, parent);
    zox_setv(e, LayoutPosition, position);
    zox_setv(e, LayoutSize, size);
    zox_setv(e, Anchor, position_anchor);
    zox_setv(e, BonusLayer, 1); // increment it above the body of the window
    zox_setv(e, FillColor, button_fill);
    zox_setv(e, OutlineColor, button_outline);
    zox_setv(e, ElementMargins, margins);
    zox_setv(e, RenderDisabled, !visible);
    entity handle = spawn_handle(
        world,
        prefab_handle,
        e,
        size,
        handle_position,
        handle_size,
        float2_half,
        handle_fill,
        handle_outline,
        !horizontal);
    zox_add(handle, ScrollbarHandle);
    if (horizontal) {
        zox_setv(handle, SlideBounds, float2_zero);
        zox_setv(e, BonusLayer, 4);
    }
    zox_set_link(world, handle, ScrollviewLink, scrollview);
    zox_setv(handle, RenderDisabled, !visible);
    return e;
}
