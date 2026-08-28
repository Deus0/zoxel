entity dbg_test_window_uis3;

void zox_dbg_test_window_uis3(ecs* world, int32_t keycode) {
    if (keycode != zox_key_k) {
        return;
    }
    zox_log("Toggling Debug UI3 [Tests]: %s", dbg_test_window_uis3 ? zox_getn(dbg_test_window_uis3) : "None");
    if (zox_valid(dbg_test_window_uis3)) {
        zox_delete(dbg_test_window_uis3);
        dbg_test_window_uis3 = 0;
        return;
    }
    entity player = dbg_player;
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    zox_log("   + spawning [dbg_test_window_uis3] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
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
        .text = "Element3",
        .on_click = { &zox_dbg_spawn_element3 },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Elements3 Corners",
        .on_click = { &zox_dbg_spawn_element3_corners },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Elements3 Tree",
        .on_click = { &zox_dbg_spawn_element3_tree },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Label3",
        .on_click = { &zox_dbg_spawn_label3 },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Popup3",
        .on_click = { &zox_dbg_spawn_popup3 },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Bar3",
        .on_click = { &zox_dbg_spawn_bar3 },
    };
    // Test our uis
    entity spawned[elements_count];
    entity3 e3 = spawn_window_list(
        world,
        prefab_window,
        player,
        "Spawn UI3s",
        header_font_size,
        list_font_size,
        (ClickEvent) { NULL },
        can_close,
        0,
        0,
        alignment,
        dbg_ui_alignment, // float2_top_right,
        list_padding,
        spawned,
        elements,
        elements_count,
        visible_count);
    zox_set_unique_name(e3.x, "dbg_test_window_uis3");
    zox_add(e3.x, NavigationWindow);
    dbg_test_window_uis3 = e3.x;
}
