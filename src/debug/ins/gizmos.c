entity dbg_ui_gizmos;

void zox_dbg_ui_gizmos(ecs* world, int32_t keycode) {
    if (keycode != zox_key_g) {
        return;
    }
    zox_log("Toggling Debug UI [Gizmos]: %s", dbg_ui_gizmos ? zox_get_name(dbg_ui_gizmos) : "None");
    if (zox_valid(dbg_ui_gizmos)) {
        zox_delete(dbg_ui_gizmos);
        dbg_ui_gizmos = 0;
        return;
    }
    entity player = dbg_player;
    if (!zox_valid(player)) {
        return;
    }
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    zox_log("   + spawning [zox_dbg_test_window] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
    // # List #
    int elements_count = 0;
    byte visible_count = 6;
    byte alignment = zox_alignment_centre;
    byte can_close = 1;
    byte header_font_size = 6 * ui_scale;
    byte list_font_size = 4 * ui_scale;
    byte2 list_padding = byte2_single(2 * ui_scale);
    // UI
    SpawnListElement elements[zox_max_test_uis];
    elements[elements_count++] = (SpawnListElement) {
        .text = "Terrain Gizmos",
        .on_click = { &zox_dbg_toggle_gizmos_terrain },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Character Gizmos",
        .on_click = { &zox_dbg_toggle_gizmos_characters },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Vode Gizmos",
        .on_click = { &zox_dbg_toggle_gizmos_vodes },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Lights Gizmos",
        .on_click = { &zox_dbg_cycle_light_debug },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Intersects",
        .on_click = { &toggle_dbg_intersect },
    };
    // Test our uis
    entity spawned[elements_count];

    entity3 e3 = spawn_window_list(
        world,
        prefab_window,
        player,
        "Gizmos",
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
    zox_set_unique_name(e3.x, "dbg_ui_gizmos");
    zox_add(e3.x, NavigationWindow);
    dbg_ui_gizmos = e3.x;
}