entity dbg_test_window_uis;

void zox_dbg_toggle_main_menu(ecs* world, ClickEventData data) {
    entity player = data.clicker;
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    entity main_menu = zox_get_child_by_id(
        world,
        canvas,
        zox_id(MenuMain));
    if (zox_valid(main_menu)) {
        close_ui_related_tooltip(world, main_menu);
        zox_delete(main_menu);
    } else {
        spawn_main_menu(
            world,
            dbg_player,
            game_name);
    }
}

void zox_dbg_test_window_uis(ecs* world, int32_t keycode) {
    if (keycode != zox_key_j) {
        return;
    }
    zox_log("Toggling Debug UI [Tests]: %s", dbg_test_window_uis ? zox_getn(dbg_test_window_uis) : "None");
    if (zox_valid(dbg_test_window_uis)) {
        zox_delete(dbg_test_window_uis);
        dbg_test_window_uis = 0;
        return;
    }
    entity player = dbg_player;
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    zox_log("   + spawning [dbg_test_window_uis] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
    // # List #
    int elements_count = 0;
    byte visible_count = 6;
    SpawnListElement elements[zox_max_test_uis];
    byte alignment = zox_alignment_centre;
    byte can_close = 1;
    byte header_font_size = 6 * ui_scale;
    byte list_font_size = 4 * ui_scale;
    byte2 list_padding = byte2_single(2 * ui_scale);
    elements[elements_count++] = (SpawnListElement) {
        .text = "Main Menu",
        .on_click = { &zox_dbg_toggle_main_menu },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Dialogue",
        .on_click = { &zox_tst_spawn_dialogue },
    };
    // 2D UI
    elements[elements_count++] = (SpawnListElement) {
        .text = "Canvas",
        .on_click = { &zox_dbg_spawn_canvas },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Element2",
        .on_click = { &zox_dbg_spawn_element },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Elements Transforms",
        .on_click = { &zox_dbg_spawn_element_transforms },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Elements Canvas",
        .on_click = { &zox_dbg_spawn_element_canvas },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Text",
        .on_click = { &zox_tst_spawn_text2 },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Bar2",
        .on_click = { &zox_dbg_spawn_ui_bar },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Window List",
        .on_click = { &zox_tst_spawn_window_list },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Render Texture",
        .on_click = { &zox_tst_render_texture },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Render Character",
        .on_click = { &zox_tst_render_texture_character2 },
    };
    // Test our uis
    entity spawned[elements_count];
    entity3 e3 = spawn_window_list(
        world,
        prefab_window,
        player,
        "Spawn UIs",
        header_font_size,
        list_font_size,
        (ClickEvent) { NULL },
        can_close,
        0,
        0,
        alignment,
        dbg_ui_alignment,
        list_padding,
        spawned,
        elements,
        elements_count,
        visible_count);
    zox_set_unique_name(e3.x, "dbg_test_window_uis");
    zox_add(e3.x, NavigationWindow);
    dbg_test_window_uis = e3.x;
}
