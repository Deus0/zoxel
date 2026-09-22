entity spawn_player_menu_stats(ecs* world, entity player) {
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity character = zox_get_link(world, player, CharacterLink);
    byte label_font_size = 5 * ui_scale;
    entity e = spawn_datagrid(
        world,
        prefab_window,
        prefab_frame,
        prefab_icon,
        prefab_label2,
        label_font_size,
        canvas,
        character,
        zox_id(Stat),
        "Status",
        default_fill_color_frame_stat,
        default_fill_color_frame_stat);
    zox_add(e, MenuStats);
    return e;
}
