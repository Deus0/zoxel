const color default_fill_color_frame_body = { 33, 63, 63, frame_alpha };


entity spawn_player_menu_body(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    if (!zox_has(character, PartLinks)) {
        return 0;
    }
    SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab_menu_body, character, canvas, canvas_size);
    data.header_zext.text = "Body";
    data.element.prefab = prefab_menu_body;
    data.icon.prefab = prefab_icon;
    data.window.user_links_id = zox_id(PartLinks);
    data.frame.texture.fill_color = default_fill_color_frame_body;
    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };
    entity_array_d* parts = create_entity_array_d(1);
    fetch_parts_recursive(world, parts, character);
    entity3 spawns[parts->size];
    entity e = spawn_window_users(world, data, texture, 0, spawns, parts->data, parts->size);
    for (int i = 0; i < parts->size; i++) {
        entity item = parts->data[i];
        entity3 frame = spawns[i];
        if (frame.x) {
            zox_set(frame.x, ItemLink, { item });
        }
        if (frame.y) {
            zox_set(frame.y, ItemLink, { item });
        }
        if (frame.z) {
            zox_set(frame.z, ItemLink, { item });
        }

        if (i == 0 && frame.y) {
            zox_set(frame.y, ClickDisabled, { 1 });
        }
        break;
    }
    dispose_entity_array_d(parts);
    return e;
}
