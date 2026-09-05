entity spawn_stats_panel(
    ecs* world,
    entity canvas,
    entity character)
{
    if (!zox_valid(canvas)) {
        zox_loge("Invalid canvas in [spawn_game_ui_stats]");
        return 0;
    }
    if (!zox_valid(character) || zox_has(character, Dead)) {
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
    byte label_font_size = ui_scale * 6;
    int2 bar_size = (int2) { 100 * ui_scale, 10 * ui_scale };
    byte bar_padding = 6 * ui_scale;
    int panel_height = total_bars * (bar_size.y + bar_padding) - bar_padding;
    byte2 panel_padding = (byte2) { 6 * ui_scale, 4 * ui_scale };
    int2 size = (int2) {
        bar_size.x + panel_padding.x * 2,
        panel_height + panel_padding.y * 2
    };
    int2 position = int2_zero; // (int2) { 16 * ui_scale, - 16 * ui_scale };
    byte header_font_size = 4 * ui_scale;
    byte2 header_padding = (byte2) { 10 * ui_scale, 4 * ui_scale };
    entity3 e2 = spawn_window(
        world,
        prefab_window,
        prefab_body,
        "",
        canvas,
        position,
        size,
        position_anchor,
        header_font_size,
        header_padding,
        NULL);
    entity e = e2.x;
    entity body = e2.z;
    zox_set_unique_name(e, "statbars");
    zox_add(e,  StatBars);
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
        spawn_statbar2(
            world,
            body,
            stat,
            bar_position,
            bar_size,
            float2_half,
            fill,
            label_font_size);
        // zox_set_parent(world, statbar, body);
        bar_position.y -= bar_size.y + bar_padding;
    }
    return e;
}

void player_state_stats_ui(ecs* world, entity player, byte state) {
    byte dbg_log = 0;
    byte is_spawn = state == zox_player_state_play_begin;
    byte is_destroy =
        state == zox_player_state_pause_begin ||
        state == zox_player_state_respawn_begin;
    if (!is_destroy && !is_spawn) {
        return;
    }
    entity canvas = zox_get_link(world, player, Canvas);
    entity character = zox_get_link(world, player, Character);
    if (!zox_valid(canvas)) {
        return;
    }
    entity statbars = zox_get_child_by_id(
        world,
        canvas,
        zox_id(StatBars));
    if (is_spawn && !zox_valid(statbars)) {
        spawn_stats_panel(
            world,
            canvas,
            character);
        if (dbg_log) {
            zox_log("Spawned Stats Panel for Player [%s]",
                zox_getn(player));
        }
    } else if (is_destroy && zox_valid(statbars)) {
        if (dbg_log) {
            zox_log("Destroying Stats Panel [%s] for Player [%s]",
                zox_getn(statbars),
                zox_getn(player));
        }
        zox_delete(statbars);
    }
}
