// TODO: Just spawn tthis when player character first loads, add new state for that
entity spawn_menu_actions(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(canvas)) {
        zox_loge("Character not found on canvas");
        return 0;
    }
    if (!zox_valid(character)) {
        zox_loge("Character not found on player");
        return 0;
    }
    entity old_menu = zox_get_child_by_id(world, canvas, zox_id(MenuActions));
    if (zox_valid(old_menu)) {
        return 0;
    }
    entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
    if (!zox_valid(actionbar)) {
        zox_loge("Actionbar Slots not found on player");
        return 0;
    }
    byte2 cells_size = (byte2) { 10, 1 };
    byte label_font_size = 5 * ui_scale;
    int frame_size = ((default_frame_size / 4) * ui_scale);
    float2 position_anchor = (float2) { 0.5f, 0 };
    int2 position = (int2) { 0, frame_size * 0.5f };
    entity frame_id = zox_id(ActionFrame);
    entity e = spawn_datagrid_slots(
        world,
        prefab_window,
        prefab_frame_selectable,
        prefab_icon,
        prefab_label2,
        label_font_size,
        canvas,
        character,
        actionbar,
        cells_size,
        "",
        fill_color_frame_action,
        outline_color_frame_action,
        position_anchor,
        position,
        frame_id);
    zox_add(e, MenuActions);
    return e;
}

byte is_action_frame(ecs* world, entity frame) {
    return zox_valid(frame) && zox_has(frame, ActionFrame);
}
