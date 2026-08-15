void button_event_close_window(ecs *world, ClickEventData event) {
    entity parent = zox_get_parent(world, event.clicked);
    entity window = zox_get_parent(world, parent);
    zox_delete(window);
}

entity spawn_prefab_button_close(ecs *world, entity prefab) {
    zox_prefab_child(prefab);
    zox_prefab_name("button_close");
    zox_add(e, CloseButton);
    zox_prefab_set(e, FrameCorner, { close_button_corner_size });
    zox_prefab_set(e, OutlineThickness, { close_button_outline_size });
    zox_prefab_set(e, ClickEvent, { &button_event_close_window });
    return e;
}
