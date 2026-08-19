// NOTE: Vertical Scrollbar!
entity spawn_scrollbar(
    ecs *world,
    entity parent,
    int2 position,
    int2 size,
    int scrollbar_margins,
    int visible_elements,
    int total_elements,
    byte visible,
    entity scrollview)
{
    float2 position_anchor = (float2) { 1.0f, 0.5f };
    int2 margins = (int2) { scrollbar_margins, 0 };
    int handle_height = !visible_elements ? size.y : (int) size.y * (float_min(1, (float) visible_elements / (float) total_elements));
    int2 handle_size = (int2) { size.x, handle_height };
    int2 handle_position = (int2) { 0, (size.y - handle_height) / 2 };
    // top of scroll
    zox_instance(prefab_scrollbar);
    zox_name("scrollbar");
    zox_set_parent(world, e, parent);
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
        1);
    zox_add(handle, ScrollbarHandle);
    zox_setv(handle, ScrollviewLink, scrollview);
    zox_setv(handle, RenderDisabled, !visible);
    return e;
}
