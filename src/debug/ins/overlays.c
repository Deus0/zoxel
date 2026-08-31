
void zox_dbg_ui_overlays(ecs* world, int32_t keycode) {
    if (keycode != zox_key_v) {
        return;
    }
    zox_log("Toggling Debug UI [Overlays]: %s", dbg_ui_overlays ? zox_get_name(dbg_ui_overlays) : "None");
    if (zox_valid(dbg_ui_overlays)) {
        zox_delete(dbg_ui_overlays);
        dbg_ui_overlays = 0;
        return;
    }
    entity player = dbg_player;
    entity canvas = zox_get_link(world, player, Canvas);
    if (!zox_valid(canvas)) {
        return;
    }
    zox_log("   + spawning [zox_dbg_test_window] on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));
    // # List #
    int elements_count = 0;
    byte visible_count = 6;
    SpawnListElement elements[zox_max_test_uis];
    byte alignment = zox_alignment_centre;
    byte can_close = 1;
    byte header_font_size = 6 * ui_scale;
    byte list_font_size = 4 * ui_scale;
    byte2 list_padding = byte2_single(2 * ui_scale);
    // UIs
    elements[elements_count++] = (SpawnListElement) {
        .text = "Systems",
        .on_click = { &zox_dbg_activate_ui_system_times },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Statistics",
        .on_click = { &zox_dbg_activate_ui_statistics },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Where is Character",
        .on_click = { &zox_dbg_activate_ui_chunk_link },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Inside Chunk",
        .on_click = { &zox_dbg_activate_ui_inside_chunk },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Terrain Meshes",
        .on_click = { &zox_dbg_activate_ui_meshes },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "File Paths",
        .on_click = { &zox_dbg_activate_ui_filepaths },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Seeds",
        .on_click = { &zox_dbg_activate_seed_ui },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Player States",
        .on_click = { &zox_dbg_activate_player_state_ui },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Player Cameras",
        .on_click = { &zox_dbg_activate_ui_cameras },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Player Terrains",
        .on_click = { &zox_dbg_activate_ui_terrains },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Player Character",
        .on_click = { &zox_dbg_activate_ui_player_character },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Canvas",
        .on_click = { &zox_dbg_activate_ui_canvas },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Raycast Voxels",
        .on_click = { &zox_dbg_activate_ui_raycasting },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Raycast Lights",
        .on_click = { &zox_dbg_activate_ui_raycast_lights },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Towns",
        .on_click = { &zox_dbg_activate_ui_towns },
    };
    elements[elements_count++] = (SpawnListElement) {
        .text = "Map Mode",
        .on_click = { &zox_dbg_map_cycle_ui },
    };
    // Test our uis
    entity spawned[elements_count];
    // int2 position = (int2) { -16, -16 };
    // float2 anchor = { 1.0f, 1.0f };

    entity3 e3 = spawn_window_list(
        world,
        prefab_window,
        player,
        "Overlays",
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
    zox_set_unique_name(e3.x, "dbg_ui_overlays");
    zox_add(e3.x, NavigationWindow);
    dbg_ui_overlays = e3.x;
}
