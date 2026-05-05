 entity zox_dbg_window_list;

void zox_tst_window_list_button(ecs *world, ClickEventData event) {
    zox_log("Clicked test window button");
}

void zox_tst_key_down_window_list(ecs *world, int32_t keycode) {
    (void) world;

    if (keycode != zox_key_h) {
        return;
    }

    entity player = dbg_player;

    zox_log("Testing [window_list]: %lu", zox_dbg_window_list);

    if (zox_dbg_window_list) {
        zox_delete(zox_dbg_window_list);
        zox_dbg_window_list = 0;
        return;
    }

    const char* header_label = "Testing";
    byte header_font_size = 10 * ui_scale;
    byte list_font_size = 8 * ui_scale;

    SpawnListElement elements[1];
    int elements_count = 0;
    int visible_count = 1;
    ClickEvent close_event = (ClickEvent) { NULL };
    byte menu_type = zox_window_main_menu;
    byte alignment = zox_alignment_centre;

    elements[elements_count++] = (SpawnListElement) {
        .text = "Testing",
        .on_click = { &zox_tst_window_list_button },
    };

    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_log("   + spawning on player %s on canvas %s", zox_get_name(player), zox_get_name(canvas));

    zox_dbg_window_list = spawn_window_list(world, prefab_window, player, header_label, header_font_size, elements, visible_count, elements_count, list_font_size, close_event, 0, menu_type, 0, alignment, byte2_single(4), NULL).x;
}
