entity spawn_scrollbar_handle(
    ecs *world,
    const entity parent,
    const entity canvas,
    const int2 position,
    const float2 anchor,
    const byte layer,
    const int2 size,
    const int2 parent_size,
    byte visible
) {
    zox_instance(prefab_scrollbar_front);
    zox_name("handle");
    initialize_element(
        world,
        e,
        parent,
        canvas,
        position,
        size,
        size,
        anchor,
        layer
    );
    zox_add_tag(e, ScrollbarButton);
    zox_set(e, RenderDisabled, { !visible });
    const int b = (parent_size.y / 2) - size.y / 2;
    zox_set(e, LayoutConstraints, { (int4) { 0, 0, -b, b } });
    return e;
}
