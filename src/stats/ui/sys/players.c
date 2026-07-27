entity spawn_menu_game_stats(ecs* world, entity canvas, entity character) {
    if (!zox_valid(canvas)) {
        zox_loge("Invalid canvas in [spawn_game_ui_stats]");
        return 0;
    }
    if (!zox_valid(character) || zox_getv(character, Dead)) {
        zox_loge("Invalid Character in [spawn_game_ui_stats]");
        return 0;
    }
    entity stats[zox_children_capacity];
    uint stats_length = zox_get_children_by_id(world, character, stats, zox_children_capacity, zox_id(Stat));
    if (!stats_length) {
        zox_logw("Character [%s] has no [Stat] children", zox_get_name(character));
        zox_logw(" - GenerateCharacter? %i", zox_getv(character, GenerateCharacter));
        return 0;
    }
    byte total_bars = 4; // TODO: Make dynamic
    // Sizing
    float2 position_anchor = float2_top_left;
    // NOTE: Font size is scaled from height of bar
    byte label_font_size = ui_scale * 5;
    int2 bar_size = (int2) { 100 * ui_scale, 10 * ui_scale };
    byte bar_padding = 2 * ui_scale;
    int panel_height = total_bars * (bar_size.y + bar_padding) - bar_padding;
    byte panel_padding = 8 * ui_scale;
    int2 size = (int2) { bar_size.x + panel_padding * 2, panel_height + panel_padding * 2 };
    int2 position = (int2) { 16 * ui_scale, - 16 * ui_scale };
    byte header_font_size = 4 * ui_scale;
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    entity3 e2 = spawn_window(world, prefab_window, prefab_body, "", canvas, position, size, position_anchor, header_font_size, header_padding, NULL);
    entity e = e2.x;
    entity body = e2.z;
    zox_set_unique_name(e, "statbars");
    zox_add_tag(e,  StatBars);
    // Now our bars
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
        color_rgb fill = zox_getv(stat, ColorRGB);
        spawn_statbar2(world, body, stat, bar_position, bar_size, float2_half, fill, label_font_size);
        // zox_set_parent(world, statbar, body);
        bar_position.y -= bar_size.y + bar_padding;
    }
    return e;
}

// NOTE: When Game Starts or Resumes we spawn the stats panel
zox_sys2(PlayerStatspanelSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CharacterLink);
    zox_sys_in(CanvasLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CharacterLink, character);
        zox_sys_i(CanvasLink, canvas);
        if (dirty->value != zox_dirty_active) {
            continue;
        }
        if (!zox_valid(canvas->value)) {
            zox_logw("Canvas is missing from Player [PlayerUIGamePauseSystem]");
            continue;
        }
        entity statbars = zox_get_child_by_id(world, canvas->value, zox_id(StatBars));
        if (state->value == zox_player_state_pause_begin || state->value == zox_player_state_respawn_begin) {
            if (zox_valid(statbars)) {
                if (dbg_log) {
                    zox_log("- Destroying Stats Panel [%s] for Player [%s]", zox_get_name(statbars), zox_get_name(e));
                }
                zox_delete(statbars);
            }
        } else if (state->value == zox_player_state_play_begin) {
            //  || state->value == zox_player_state_playing
            // NOTE: Because character
            if (!zox_valid(statbars)) {
                if (dbg_log) {
                    zox_log("+ Spawning Stats Panel for Player [%s]", zox_get_name(e));
                }
                spawn_menu_game_stats(world, canvas->value, character->value);
            }
        }
    }
} zox_sys_end(PlayerStatspanelSystem);
