static void window_undock_click(ecs* world, ClickEventData event) {
    const entity header = zox_get_parent(world, event.clicked);
    const entity window = zox_get_parent(world, header);
    if (zox_has(window, Docked)) {
        dock_undock(world, window);
    }
}

static void window_undock_button(ecs* world, entity window, entity header) {
    const byte docked = zox_has(window, Docked);
    entity button = zox_get_child_by_id(world, header, zox_id(UndockButton));
    if (docked && !zox_get_child_by_id(world, header, zox_id(CloseButton))) {
        spawn_close_button(world, header, 0, 18, 4, 0, (ClickEvent) { button_event_close_window });
    }
    if (docked && !button) {
        button = spawn_button(world, prefab_clickable, header, "<",
            (int2) { -44, 0 }, (int2) { 20, 20 }, dock_right, zox_alignment_centre,
            12, byte2_single(0), header_fill, header_outline, header_font_fill, header_font_outline).x;
        zox_setv(button, ClickEvent, window_undock_click);
        zox_add(button, UndockButton);
    }
    if (button) {
        zox_setv(button, RenderDisabled, !docked);
    }
}
