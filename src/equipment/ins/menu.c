const color default_fill_color_frame_equip = { 63, 43, 33, frame_alpha };

entity spawn_player_menu_equipment(ecs* world, entity player) {

    zox_geter_value(player, CanvasLink, entity, canvas);

    if (!zox_valid(canvas)) {
        return 0;
    }

    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);

    if (!zox_has(character, EquipLinks)) {
        return 0;
    }

    SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab_menu_equipment, character, canvas, canvas_size);

    data.header_zext.text = "Equipment";

    data.element.prefab = prefab_menu_equipment;

    data.icon.prefab = prefab_icon_item;

    data.window.user_links_id = zox_id(EquipLinks);

    data.frame.texture.fill_color = default_fill_color_frame_equip;

    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };

    zox_geter(character, EquipLinks, links);
    entity3 spawns[links->length];

    entity e = spawn_window_users_id(world, data, texture, 0, spawns);

    for (uint i = 0; i < links->length; i++) {
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
