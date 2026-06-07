entity spawn_scrollbar_handle(ecs *world, entity p, entity parent, entity canvas, int2 position, float2 anchor, byte layer, int2 size, int2 parent_size, byte visible) {
    zox_instance(p);
    zox_name("scrollbar_handle");
    initialize_element(world, e, parent, canvas, position, size, size, anchor, layer);
    zox_add_tag(e, ScrollbarHandle);
    zox_set(e, RenderDisabled, { !visible });
    int b = (parent_size.y / 2) - size.y / 2;
    zox_set(e, LayoutConstraints, { (int4) { 0, 0, -b, b } });
    zox_set(e, FillColor, { button_fill });
    zox_set(e, OutlineColor, { button_outline });
    return e;
}
