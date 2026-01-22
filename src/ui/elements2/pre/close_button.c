void button_event_close_window(ecs *world, const ClickEventData event) {
    zox_geter_value(event.clicked, ParentLink, entity, parent);
    zox_geter_value(parent, ParentLink, entity, window);

    zox_delete(window);
}

entity spawn_prefab_button_close(ecs *world, entity p) {
    zox_prefab_child(p);
    zox_prefab_name("button_close");
    zox_add_tag(e, CloseButton);
    zox_prefab_set(e, ClickEvent, { &button_event_close_window });
    return e;
}