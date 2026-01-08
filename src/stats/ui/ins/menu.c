entity spawn_player_menu_stats(ecs* world, const entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);

    SpawnWindowUsers data = get_default_spawn_window_users_data(
        world,
        prefab_menu_stats,
        character,
        canvas,
        canvas_size
    );
    data.header_zext.text = "Status";
    data.element.prefab = prefab_menu_stats;
    data.icon.prefab = prefab_icon_stat;
    data.window.user_links_id = zox_id(StatLinks);
    data.frame.texture.fill_color = default_fill_color_frame_stat;

    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };

    zox_geter(character, StatLinks, stats);
    entity3 spawns[stats->length];

    entity e = spawn_window_users(
        world,
        data,
        texture,
        0,
        spawns
    );

    if (!e) {
        zox_logw("Stats UI spawning failed.");
        return 0;
    }

    for (int i = 0; i < stats->length; i++) {
        entity stat = stats->value[i];
        entity3 frame = spawns[i];
        if (frame.x) {
            zox_set(frame.x, StatLink, { stat });
        }
        if (frame.y) {
            zox_set(frame.y, StatLink, { stat });
        }
        if (frame.z) {
            zox_set(frame.z, StatLink, { stat });
        }
    }

    return e;
}
