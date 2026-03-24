entity spawn_menu_actions(ecs *world, entity player) {

    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character) || !zox_has(character, ActionIndex) || !zox_has(character, ActionLinks)) {
        zox_log_error("[!spawn_menu_actions] invalid character");
        return 0;
    }

    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    zox_geter(character, ActionLinks, actions);
    zox_geter_value(character, ActionIndex, byte, selected);

    // Sizing
    byte2 grid_padding = (byte2) { 3 * ui_scale, 0 };
    byte2 grid_margins = (byte2) { 5 * ui_scale, 2 * ui_scale };
    int2 screen_margins = (int2) { 0, 4 * ui_scale };
    byte header_buffer = ui_scale * 4;

    // Misc
    byte2 grid_size = (byte2) { 8, 1 };

    SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab_menu_actions, character, canvas, canvas_size);

    // prefabs
    data.frame.prefab = prefab_frame_action;
    data.icon.prefab = prefab_icon_action;
    // window
    data.element.prefab = prefab_menu_actions;
    data.element.anchor = (float2) { 0.5f, 0 };
    data.window.user_links_id = zox_id(ActionLinks);
    // header
    data.header_zext.text = "";
    data.header_zext.font_size = header_buffer;
    data.header_zext.margins.y = 0;
    // grid
    data.window.grid_size = grid_size;
    data.window.grid_padding = grid_padding;
    data.window.grid_margins = grid_margins;

    int header_height = data.header_zext.font_size + data.header_zext.margins.y * 2;
    data.element.size = calculate_grid_window_size(data.window, header_height);
    data.element.position = (int2) { 0, data.element.size.y / 2 + screen_margins.y };
    data.frame.texture.fill_color = fill_color_frame_action;
    data.frame.texture.outline_color = outline_color_frame_action;
    FrameTextureData texture = (FrameTextureData) {
        .fill_color = fill_color_actionbar,
        .outline_color = outline_color_actionbar
    };

    entity3 spawns[actions->length];
    entity e = spawn_window_users_id(world, data, texture, selected, spawns);

    if (!e) {
        zox_log_error("Failed spawning Actionbar");
        return 0;
    }
    zox_set_unique_name(e, "actionbar");

    for (int i = 0; i < actions->length; i++) {
        entity action = actions->value[i];
        entity3 frame = spawns[i];
        if (frame.x) {
            zox_set(frame.x, ItemLink, { action });
        }
        if (frame.y) {
            zox_set(frame.y, ItemLink, { action });
        }
        if (frame.z) {
            zox_set(frame.z, ItemLink, { action });
        }
    }

    return e;
}
