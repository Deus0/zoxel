// The Window items are in the same order as these debug shortcuts.
static const int32_t editor_window_keys[] = {
    zox_key_y, zox_key_u, zox_key_n, zox_key_x, zox_key_z
};

static void editor_menu_toggle(ecs* world, ClickEventData event) {
    const uint index = zox_getv(event.clicked, ChildIndex);
    switch (editor_window_keys[index]) {
        case zox_key_y: toggle_hierarchy(world, zox_key_y); break;
        case zox_key_u: toggle_inspector(world, zox_key_u); break;
        case zox_key_n: toggle_files(world, zox_key_n); break;
        case zox_key_x: toggle_profiler(world, zox_get_link(world, event.clicker, CanvasLink)); break;
        case zox_key_z: toggle_fps_viewer(world, zox_key_z); break;
    }
}

static void editor_menu_open_files(ecs* world, ClickEventData event) {
    (void) event;
    toggle_files(world, zox_key_n);
}

static void editor_menu_quit(ecs* world, ClickEventData event) {
    (void) world;
    (void) event;
    disable_time_pausing();
    engine_end();
}

static void editor_realm_label(ecs* world, entity item, byte has_realm) {
    const entity label = zox_get_child_by_id(world, item, zox_id(TextLabel));
    set_entity_text(world, label, has_realm ? "Dispose Realm" : "Spawn Realm");
}

static void editor_menu_realm(ecs* world, ClickEventData event) {
    const entity game = zox_get_link(world, event.clicker, GameLink);
    const entity canvas = zox_get_link(world, event.clicker, CanvasLink);
    if (!zox_valid(game) || !zox_valid(canvas)) {
        return;
    }
    const entity realm = zox_get_link(world, game, RealmLink);
    const byte dispose = zox_valid(realm);
    if (dispose) {
        local_terrain = 0;
        local_realm = 0;
        zox_delete(realm);
    } else {
        spawn_new_realm(world, event.clicker);
        zox_setv(local_realm, GenerateRealm, zox_generate_realm_start);
    }
    editor_realm_label(world, event.clicked, !dispose);
    const entity hierarchy = zox_get_child_by_id(world, canvas, zox_id(HierarchyUI));
    if (hierarchy) {
        zox_add(get_editor_window_list(world, hierarchy), Dirty);
    }
}

static void editor_window_menu_refresh(ecs* world, entity menu) {
    const entity canvas = zox_get_link(world, menu, ItemListCanvas);
    const byte checked[] = {
        !!zox_get_child_by_id(world, canvas, zox_id(HierarchyUI)),
        !!get_canvas_inspector(world, canvas),
        !!zox_get_child_by_id(world, canvas, zox_id(FileWindowUI)),
        !!zox_get_child_by_id(world, canvas, zox_id(Profiler)),
        zox_fps_display
    };
    for (uint i = 0; i < sizeof(checked) / sizeof(*checked); i++) {
        const entity item = item_list_item_at(world, menu, i);
        if (item && zox_getv(item, ItemListChecked) != checked[i]) {
            item_list_set_checked(world, item, checked[i]);
        }
    }
}

static void editor_window_menu_click(ecs* world, ClickEventData event) {
    editor_window_menu_refresh(world, event.clicked);
    item_list_toggle(world, event);
}

static void editor_realm_menu_click(ecs* world, ClickEventData event) {
    const entity game = zox_get_link(world, event.clicker, GameLink);
    editor_realm_label(world, item_list_item_at(world, event.clicked, 0),
        zox_valid(game) && zox_valid(zox_get_link(world, game, RealmLink)));
    item_list_toggle(world, event);
}

static void editor_disabled_menu_items(ecs* world, entity menu, const char* const* names, int count) {
    for (int i = 0; i < count; i++) {
        item_list_set_disabled(world, spawn_menu_item(world, menu, names[i], (ClickEvent) { 0 }), 1);
    }
}

static void spawn_editor_menubar(ecs* world, entity canvas) {
    const int2 size = { zox_getv(canvas, LayoutSize).x, 24 };
    const entity bar = spawn_menubar(world, canvas, canvas, size, (int2) { 0, -12 }, dock_top);
    zox_add(bar, EditorElement);
    zox_add(bar, Docked);

    const entity file = spawn_menubar_menu(world, bar, "File");
    spawn_menu_item(world, file, "Open", (ClickEvent) { editor_menu_open_files });
    editor_disabled_menu_items(world, file, (const char*[]) { "Save" }, 1);
    spawn_menu_item(world, file, "Quit", (ClickEvent) { editor_menu_quit });

    const entity edit = spawn_menubar_menu(world, bar, "Edit");
    editor_disabled_menu_items(world, edit, (const char*[]) { "Undo", "Redo", "Copy", "Paste" }, 4);

    const entity window = spawn_menubar_menu(world, bar, "Window");
    zox_add(window, EditorWindowMenu);
    zox_setv(window, ClickEvent, editor_window_menu_click);
    const char* windows[] = { "Hierarchy", "Inspector", "Files", "Profiler", "FPS" };
    for (uint i = 0; i < sizeof(windows) / sizeof(*windows); i++) {
        const entity item = spawn_menu_item(world, window, windows[i], (ClickEvent) { editor_menu_toggle });
        item_list_set_checked(world, item, 0);
    }

    const entity realm = spawn_menubar_menu(world, bar, "Realm");
    zox_setv(realm, ClickEvent, editor_realm_menu_click);
    spawn_menu_item(world, realm, "Spawn Realm", (ClickEvent) { editor_menu_realm });
    editor_disabled_menu_items(world, realm, (const char*[]) { "Play", "Pause", "Stop" }, 3);
    zox_add(bar, EditorThemeDirty);
}
