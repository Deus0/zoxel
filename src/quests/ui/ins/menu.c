entity spawn_player_menu_quests(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    color frame_fill = { 33, 33, 33, frame_alpha };
    color frame_outline = { 222, 222, 222, frame_alpha };
    byte label_font_size = 3 * ui_scale;
    entity e = spawn_datagrid(world, prefab_window, prefab_frame, prefab_icon, prefab_label2, label_font_size, canvas, character, zox_id(Quest), "Quests", frame_fill, frame_outline);
    zox_add(e, MenuQuests);
    return e;
}
