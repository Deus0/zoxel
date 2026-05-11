entity spawn_menu_game_stats(ecs* world, entity parent, entity player) {
    if (!player || !zox_has(player, CharacterLink) || !zox_has(player, CanvasLink)) {
        zox_log_error("! invalid player in [spawn_game_ui_stats]");
        return 0;
    }
    zox_geter_value(player, CanvasLink, entity, canvas);
    if (!canvas) {
        zox_log_error("! invalid canvas in [spawn_game_ui_stats]");
        return 0;
    }
    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character)) {
        zox_log_error("Invalid Character [%lu] in [spawn_game_ui_stats]", character);
        return 0;
    }
    /*if (!zox_has(character, StatLinks)) {
        zox_log_error("Invalid Character - No StatLinks [%s] in [spawn_game_ui_stats]", zox_get_name(character));
        return 0;
    }*/
    byte total_bars = 4; // TODO: Make dynamic
    // Sizing
    byte panel_padding = 6 * ui_scale;
    int2 panel_size = (int2) { 80 * ui_scale, 0 };
    // NOTE: Font size is scaled from height of bar
    int2 bar_size = (int2) { 75 * ui_scale, 8 * ui_scale };
    byte bar_padding = 2 * ui_scale;
    int panel_height = total_bars * (bar_size.y + bar_padding) - bar_padding;
    panel_size.y = panel_height + panel_padding * 2;
    byte2 screen_padding = (byte2) { 4 * ui_scale, 4 * ui_scale };
    byte label_font_size = ui_scale * 4;
    // Positioning
    float2 panel_anchor = float2_top_left;
    float2 bar_anchor = float2_half;
    int2 bar_position = (int2) { 0, - bar_size.y / 2 + panel_height / 2 };
    int2 panel_position = (int2) {
        panel_size.x / 2 + screen_padding.x,
        -panel_size.y / 2 - screen_padding.y
    };
    // Others
    entity stats[stats_children_capacity];
    uint stats_length = zox_get_children_by_id(world, character, stats, stats_children_capacity, zox_id(Stat));
    if (!stats_length) {
        zox_logw("[%s] has no stats", zox_get_name(parent));
        return 0;
    }
    // zox_geter(character, StatLinks, stats);
    entity e = spawn_ui(world, prefab_body, parent, panel_anchor, panel_position, panel_size, panel_size);
    zox_set_unique_name(e, "stats_panel");
    zox_set_parent(world, e, parent);
    for (uint i = 0; i < stats_length; i++) {
        entity stat = stats[i];
        if (!zox_valid(stat)) {
            continue;
        }
        if (!zox_has(stat, StatState) && !zox_has(stat, StatLevel)) {
            continue;
        }
        if (!zox_has(stat, ColorRGB)) {
            zox_loge("Stat [%s] has no ColorRGB", zox_get_name(stat));
            continue;
        }
        zox_geter_value(stat, ColorRGB, color_rgb, cvalue);
        entity statbar = spawn_statbar2(world, canvas, e, (entity2) { character, stat }, cvalue, 0, bar_anchor, bar_size, bar_position, label_font_size        );
        zox_set_parent(world, statbar, e);
        bar_position.y -= bar_size.y + bar_padding;
    }
    return e;
}
