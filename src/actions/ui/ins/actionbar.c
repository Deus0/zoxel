entity spawn_menu_actions(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
    byte2 cells_size = (byte2) { 10, 1 };
    byte label_font_size = 5 * ui_scale;
    int frame_size = ((default_frame_size / 4) * ui_scale);
    float2 position_anchor = (float2) { 0.5f, 0 };
    int2 position = (int2) { 0, frame_size * 1.5f };
    entity e = spawn_datagrid3(world, prefab_window, prefab_frame_selectable, prefab_icon, prefab_label, label_font_size, canvas, character, actionbar, cells_size, "", fill_color_frame_action, outline_color_frame_action, position_anchor, position);
    zox_add_tag(e, MenuActions);
    return e;
}
