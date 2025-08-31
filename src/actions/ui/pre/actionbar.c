entity spawn_player_menu_actions(
    ecs *world,
    const entity player
) {
    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character)) {
        zox_log_error("[!spawn_player_menu_actions] invalid character");
        return 0;
    }
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    zox_geter_value(character, ActionIndex, byte, selected);
    SpawnWindowUsers data = get_default_spawn_window_users_data(
        world,
        prefab_menu_actions,
        character,
        canvas,
        canvas_size);
    // window
    data.element.prefab = prefab_menu_actions;
    data.element.anchor = (float2) { 0.5f, 0 };
    data.window.user_links_id = zox_id(ActionLinks);
    // header
    data.header_zext.text = "";
    data.header_zext.font_size = 4;
    // grid
    data.window.grid_size = (byte2) { 8, 1 };
    data.window.grid_padding.x = 12;
    data.window.grid_padding.y = 0;
    data.window.grid_margins.x = 20;
    data.window.grid_margins.y = 8;
    // elements
    data.frame.prefab = prefab_frame_action;
    data.icon.prefab = prefab_icon_action;
    int header_height = data.header_zext.font_size + data.header.margins * 2;
    data.element.size = (int2) {
        data.window.grid_padding.x + (data.window.icon_size + data.window.grid_padding.x) * data.window.grid_size.x + data.window.grid_margins.x * 2,
        data.window.grid_padding.y + (data.window.icon_size + data.window.grid_padding.y) * data.window.grid_size.y + data.window.grid_margins.y * 2 + header_height
    };
    data.element.position = (int2) { 0, data.element.size.y / 2 + 16 };
    data.frame.texture.fill_color = fill_color_frame_action;
    data.frame.texture.outline_color = outline_color_frame_action;
    FrameTextureData texture = (FrameTextureData) {
        .fill_color = fill_color_actionbar,
        .outline_color = outline_color_actionbar
    };


    zox_geter(character, ActionLinks, actions);
    entity3 spawns[actions->length];

    entity e = spawn_window_users(
        world,
        data,
        texture,
        selected,
        spawns
    );
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