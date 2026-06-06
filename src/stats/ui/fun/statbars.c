entity spawn_menu_game_stats(ecs* world, entity player) {
    if (!player || !zox_has(player, CharacterLink) || !zox_has(player, CanvasLink)) {
        zox_loge("Invalid player in [spawn_game_ui_stats]");
        return 0;
    }
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    if (!canvas) {
        zox_loge("Invalid canvas in [spawn_game_ui_stats]");
        return 0;
    }
    if (!zox_valid(character)) {
        zox_loge("Player [%s] has no Character in [spawn_game_ui_stats]", zox_get_name(player));
        return 0;
    }
    // Others
    entity stats[stats_children_capacity];
    uint stats_length = zox_get_children_by_id(world, character, stats, stats_children_capacity, zox_id(Stat));
    if (!stats_length) {
        zox_logw("[%s] has no stats", zox_get_name(character));
        return 0;
    }
    byte total_bars = 4; // TODO: Make dynamic
    // Sizing
    byte panel_padding = 6 * ui_scale;
    // NOTE: Font size is scaled from height of bar
    int2 bar_size = (int2) { 75 * ui_scale, 8 * ui_scale };
    byte bar_padding = 2 * ui_scale;
    int panel_height = total_bars * (bar_size.y + bar_padding) - bar_padding;
    int2 size = (int2) { 80 * ui_scale, panel_height + panel_padding * 2 };
    // Positioning
    float2 position_anchor = float2_top_left;
    int2 position = (int2) { 8 * ui_scale, - 6 * ui_scale };
    entity2 e2 = spawn_window(world, prefab_window, prefab_body, "", canvas, position, size, position_anchor, NULL);
    entity e = e2.x;
    entity body = e2.y;
    zox_set_unique_name(e, "statbars");
    zox_add_tag(e, StatBars);
    // Now our bars
    byte label_font_size = ui_scale * 4;
    int2 bar_position = (int2) { 0, - bar_size.y / 2 + panel_height / 2 };
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
        entity statbar = spawn_statbar2(world, canvas, body, (entity2) { character, stat }, cvalue, 0, float2_half, bar_size, bar_position, label_font_size);
        zox_set_parent(world, statbar, body);
        bar_position.y -= bar_size.y + bar_padding;
    }
    return e;
}
