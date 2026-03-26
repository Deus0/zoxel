entity spawn_scrollbar_handle(ecs *world, entity parent, entity canvas, int2 position, float2 anchor, byte layer, int2 size, int2 parent_size, byte visible) {

    zox_instance(prefab_scrollbar_front);
    zox_name("scrollbar_handle");

    initialize_element(world, e, parent, canvas, position, size, size, anchor, layer);
    zox_add_tag(e, ScrollbarHandle);

    zox_set(e, RenderDisabled, { !visible });

    int b = (parent_size.y / 2) - size.y / 2;
    zox_set(e, LayoutConstraints, { (int4) { 0, 0, -b, b } });

    return e;
}
