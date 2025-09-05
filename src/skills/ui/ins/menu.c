entity spawn_player_menu_skills(
    ecs* world,
    const entity player
) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);

    SpawnWindowUsers spawn_data = get_default_spawn_window_users_data(
        world,
        prefab_menu_skills,
        character,
        canvas,
        canvas_size
    );
    spawn_data.header_zext.text = "Skillbook";
    // spawn_data.element.prefab = prefab_menu_items;
    spawn_data.element.prefab = prefab_menu_skills;
    spawn_data.icon.prefab = prefab_icon_skill;
    spawn_data.window.user_links_id = zox_id(SkillLinks);
    spawn_data.frame.texture.fill_color = default_fill_color_frame_skill;

    FrameTextureData texture_data = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };

    zox_geter(character, SkillLinks, skills);
    entity3 spawns[skills->length];

    entity e = spawn_window_users(
        world,
        spawn_data,
        texture_data,
        0,
        spawns
    );

    for (int i = 0; i < skills->length; i++) {
        entity e2 = skills->value[i];
        entity3 frame = spawns[i];
        if (frame.x) {
            zox_set(frame.x, SkillLink, { e2 });
        }
        if (frame.y) {
            zox_set(frame.y, SkillLink, { e2 });
        }
        if (frame.z) {
            zox_set(frame.z, SkillLink, { e2 });
        }
    }

    return e;
}
