// the player's stat bars each hide after this many seconds without changing (no regen, no damage)
const float statbars_hide_time = 5;
// the panel: its bars stacked, a gap between, a padding round them (a fitting list: hidden bars' gaps close)
#define statbars_bar_padding ((int) (6 * ui_scale))
#define statbars_panel_padding_y ((int) (4 * ui_scale))

entity spawn_stats_panel(
    ecs* world,
    entity canvas,
    entity character,
    int top)
{
    if (!zox_valid(canvas)) {
        zox_loge("Invalid canvas in [spawn_game_ui_stats]");
        return 0;
    }
    if (!zox_valid(character) ||
        zox_has(character, Dead))
    {
        zox_loge("Invalid Character in [spawn_game_ui_stats]");
        return 0;
    }
    entity stats[zox_children_capacity];
    uint stats_length = zox_get_children_by_id(
        world,
        character,
        stats,
        zox_children_capacity,
        zox_id(Stat));
    if (!stats_length) {
        zox_logw("Character [%s] has no [Stat] children", zox_get_name(character));
        zox_logw(" - GenerateCharacter? %i",
            zox_getv(character, GenerateCharacter));
        return 0;
    }
    // the soul (its level) first, then health, then the other bars in their order: the children's order isn't fixed
    //  (a stable sort by kind)
    entity ordered[zox_children_capacity];
    uint ordered_length = 0;
    for (byte kind = 0; kind < 3; kind++) {
        for (uint i = 0; i < stats_length; i++) {
            const byte stat_kind = zox_has(stats[i], StatLevel) ? 0 : zox_has(stats[i], StatHealth) ? 1 : 2;
            if (stat_kind == kind) {
                ordered[ordered_length++] = stats[i];
            }
        }
    }
    memcpy(stats, ordered, sizeof(entity) * ordered_length);
    byte total_bars = 4; // TODO: Make dynamic
    // Sizing
    float2 position_anchor = float2_top_left;
    // NOTE: Font size is scaled from height of bar
    byte label_font_size = ui_scale * 6;
    int2 bar_size = (int2) {
        100 * ui_scale,
        10 * ui_scale
    };
    byte bar_padding = statbars_bar_padding;
    int panel_height = total_bars * (bar_size.y + bar_padding) - bar_padding;
    byte2 panel_padding = (byte2) {
        6 * ui_scale,
        statbars_panel_padding_y
    };
    int2 size = (int2) {
        bar_size.x + panel_padding.x * 2,
        panel_height + panel_padding.y * 2
    };
    int2 position = (int2) { 0, -top };   // under the touch ui's top row
    byte header_font_size = 4 * ui_scale;
    byte2 header_padding = (byte2) {
        10 * ui_scale,
        4 * ui_scale
    };
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
    // its bars a list fitting the shown ones (hidden bars' gaps close, the window shrinks, its top kept)
    zox_setv(body, ListPadding, ((byte2) { 0, statbars_bar_padding }));
    zox_setv(body, ListMargins, ((byte2) { panel_padding.x, panel_padding.y }));
    zox_setv(body, ListStart, 0);
    zox_setv(body, ListAlignment, zox_alignment_centre);
    zox_add(body, ListFit);
    zox_add(body, ListResizesParent);
    zox_add(body, ListPositionDirty);
    // Now our bars
    int2 bar_position = (int2) {
        0,
        - bar_size.y / 2 + panel_height / 2
    };
    for (uint i = 0; i < stats_length; i++) {
        entity stat = stats[i];
        if (!zox_valid(stat)) {
            continue;
        }
        if (!zox_has(stat, StatState) &&
            !zox_has(stat, StatLevel))
        {
            continue;
        }
        if (!zox_has(stat, ColorRGB)) {
            zox_loge("Stat [%s] has no ColorRGB", zox_get_name(stat));
            continue;
        }
        color_rgb fill = zox_getv(stat, ColorRGB);
        const entity bar = spawn_statbar2(
            world,
            body,
            stat,
            bar_position,
            bar_size,
            float2_half,
            fill,
            label_font_size);
        // each bar hidden while its stat doesn't change, shown while it does (statbar_system pokes it), but the soul
        //  (its level) always shown; its slot kept for the list's order
        if (bar) {
            zox_setv(bar, ChildIndex, i);
            if (!zox_has(stat, StatLevel)) {
                zox_setv(bar, AutoHide, statbars_hide_time);
                zox_setv(bar, AutoHideTime, zox_current_time);
            }
        }
        // zox_set_parent(world, statbar, body);
        bar_position.y -= bar_size.y + bar_padding;
    }
    return e;
}

void player_state_stats_ui(
    ecs* world,
    entity player,
    byte state)
{
    byte dbg_log = 0;
    byte is_spawn = state == zox_player_state_play_begin;
    byte is_destroy =
    state == zox_player_state_pause_begin ||
    state == zox_player_state_respawn_begin;
    if (!is_destroy && !is_spawn) {
        return;
    }
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(canvas)) {
        return;
    }
    entity statbars = zox_get_child_by_id(
        world,
        canvas,
        zox_id(StatBars));
    if (is_spawn && !zox_valid(statbars)) {
        const byte mode = zox_getv(player, DeviceMode);
        const byte touch = mode == zox_device_mode_touchscreen || zox_dbg_touch_with_mouse;
        spawn_stats_panel(
            world,
            canvas,
            character,
            touch ? touch_top_row_height(world, canvas) : 0);
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
