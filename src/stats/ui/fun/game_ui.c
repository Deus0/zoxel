entity spawn_menu_game_stats(
    ecs* world,
    const entity parent,
    const entity player,
    Children* parent_children
) {
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
    if (!zox_has(character, StatLinks)) {
        zox_log_error("Invalid Character - No StatLinks [%s] in [spawn_game_ui_stats]", zox_get_name(character));
        return 0;
    }

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
    zox_geter(character, StatLinks, stats);
    byte panel_layer = 1;
    byte bar_layer = 2;
    FrameTextureData panel_texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline,
    };

    ElementSpawn body_data = {
        .texture = panel_texture,
        .canvas = {
            .e = canvas,
            .size = zox_gett_value(canvas, LayoutSize),
        },
        .parent = {
            .e = parent,
            .position = int2_half(zox_gett_value(canvas, LayoutSize)),
            .size = zox_gett_value(canvas, LayoutSize)
        },
        .element = {
            .prefab = prefab_body,
            .layer = panel_layer,
            .anchor = panel_anchor,
            .position = panel_position,
            .size = panel_size
        },
    };

    const entity e = spawn_element(world, &body_data);
    add_to_Children(parent_children, e);

    Children children = { 0 };
    for (int i = 0; i < stats->length; i++) {
        entity stat = stats->value[i];
        if (!zox_has(stat, StatState) && !zox_has(stat, StatLevel)) {
            continue;
        }
        zox_geter_value(stat, ColorRGB, color_rgb, cvalue);
        entity statbar = spawn_statbar2(
            world,
            canvas,
            e,
            (entity2) { character, stat },
            cvalue,
            bar_layer,
            bar_anchor,
            bar_size,
            bar_position,
            label_font_size
        );
        add_to_Children(&children, statbar);
        bar_position.y -= bar_size.y + bar_padding;
    }
    zox_set_ptr(e, Children, children);

    return e;
}