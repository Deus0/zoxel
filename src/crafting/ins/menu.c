const color default_fill_color_frame_crafting = { 233, 233, 233, frame_alpha };

entity spawn_player_menu_crafting(ecs* world, entity player) {

    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);

    if (!zox_has(character, CraftLinks)) {
        return 0;
    }

    SpawnWindowUsers data = get_default_spawn_window_users_data(
        world,
        prefab_menu_crafting,
        character,
        canvas,
        canvas_size
    );
    data.header_zext.text = "Craft";
    data.element.prefab = prefab_menu_crafting;
    data.window.user_links_id = zox_id(CraftLinks);
    data.frame.texture.fill_color = default_fill_color_frame_crafting;

    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };

    zox_geter(character, CraftLinks, links);
    entity3 spawns[links->length];

    entity e = spawn_window_users_id(world, data, texture, 0, spawns);

    for (int i = 0; i < links->length; i++) {
        entity item = links->value[i];
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
    }

    return e;
}
