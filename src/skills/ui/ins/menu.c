entity spawn_player_menu_skills(ecs* world, entity player) {
    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character)) {
        zox_loge("Player has no Character", zox_get_name(player));
        return 0;
    }
    entity skillbook = zox_get_child_by_id(world, character, zox_id(Skillbook));
    if (!zox_valid(skillbook)) {
        zox_loge("Player Character has no Skillbook [%s]", zox_get_name(character));
        return 0;
    }
    zox_geter_value(player, CanvasLink, entity, canvas);
    byte2 cells_size = byte2_single(4);
    byte label_font_size = 5 * ui_scale;
    float2 position_anchor = float2_half;
    int2 position = int2_zero;
    entity frame_id = zox_id(SkillFrame);
    entity e = spawn_datagrid_slots(world, prefab_window, prefab_frame, prefab_icon, prefab_label, label_font_size, canvas, character, skillbook, cells_size, "Skillbook", default_fill_color_frame_skill, default_fill_color_frame_skill, position_anchor, position, frame_id);
    zox_add_tag(e, MenuSkills);
    return e;
}
