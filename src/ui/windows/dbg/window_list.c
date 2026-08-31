 entity zox_dbg_window_list;

void zox_tst_window_list_button(ecs *world, ClickEventData event) {
    zox_log("Closed WindowList [%s]", zox_get_name(zox_dbg_window_list));
    zox_dbg_window_list = 0;
}

void zox_tst_spawn_window_list(
    ecs *world,
    ClickEventData data)
{
    entity player = dbg_player;
    if (zox_dbg_window_list) {
        zox_log("Disposing Test: Spawn [WindowList] - [%s]", zox_get_name(zox_dbg_window_list));
        zox_delete(zox_dbg_window_list);
        zox_dbg_window_list = 0;
        return;
    }
    const char* header_label = "Zox Tests";
    byte header_font_size = 10 * ui_scale;
    byte list_font_size = 8 * ui_scale;
    byte can_close = 1;
    SpawnListElement elements[1];
    int elements_count = 0;
    int visible_count = 1;
    ClickEvent close_event = (ClickEvent) { NULL };
    byte menu_type = 0; // zox_window_main_menu;
    byte alignment = zox_alignment_centre;
    elements[elements_count++] = (SpawnListElement) {
        .text = "Testing",
        .on_click = { &zox_tst_window_list_button },
    };
    entity canvas = zox_get_link(world, player, Canvas);
    entity e = spawn_window_list(
        world,
        prefab_window,
        player,
        header_label,
        header_font_size,
        list_font_size,
        close_event,
        can_close,
        menu_type,
        0,
        alignment,
        float2_half,
        byte2_single(4),
        NULL,
        elements,
        visible_count,
        elements_count).x;
    zox_dbg_window_list = e;
    zox_log("Running Test: Spawn [WindowList]");
    zox_log("   - Player [%s]", zox_get_name(player));
    zox_log("   - Canvas [%s]", zox_get_name(canvas));
}
