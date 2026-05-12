entity spawn_player_menu_stats(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    return spawn_datagrid(world, prefab_menu_stats, prefab_icon_stat, canvas, character, zox_id(Stat), zox_id(StatLink), "Status", default_fill_color_frame_stat, default_fill_color_frame_stat);
    /*zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab_menu_stats, character, canvas, canvas_size);
    data.header_zext.text = "Status";
    data.element.prefab = prefab_menu_stats;
    data.icon.prefab = prefab_icon_stat;
    // data.window.user_links_id = zox_id(StatLinks);
    data.frame.texture.fill_color = default_fill_color_frame_stat;
    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };
    // zox_geter(character, StatLinks, stats);
    entity stats[stats_children_capacity];
    uint stats_length = zox_get_children_by_id(world, character, stats, stats_children_capacity, zox_id(Stat));
    entity3 spawns[stats_length];
    entity e = spawn_window_users(world, data, texture, 0, spawns, stats, stats_length);
    if (!zox_valid(e)) {
        zox_logw("Stats UI spawning failed.");
        return 0;
    }
    for (uint i = 0; i < stats_length; i++) {
        entity stat = stats[i];
        entity3 frame = spawns[i];
        if (zox_valid(frame.x)) {
            zox_set(frame.x, StatLink, { stat });
        }
        if (zox_valid(frame.y)) {
            zox_set(frame.y, StatLink, { stat });
        }
        if (zox_valid(frame.z)) {
            zox_set(frame.z, StatLink, { stat });
        }
    }
    return e;*/
}
