entity spawn_menu_inventory(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    entity inventory = zox_get_child_by_id(world, character, zox_id(Inventory));
    return spawn_datagrid3(world, prefab_menu_items, prefab_icon_item, canvas, character, inventory, zox_id(Item), zox_id(ItemLink), "Inventory", default_fill_color_frame_item, default_fill_color_frame_item);
}

/*entity spawn_menu_inventory(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab_menu_items, character, canvas, canvas_size);
    data.header_zext.text = "Inventory";
    data.element.prefab = prefab_menu_items;
    data.icon.prefab = prefab_icon_item;
    FrameTextureData texture = (FrameTextureData) {
        .fill_color = color_white, // window_fill,
        .outline_color = color_black // window_outline
    };
    data.frame.texture.fill_color = color_white;
    data.frame.texture.outline_color = color_black;
    data.window.user_links_id = zox_id(ItemLinks); //
    zox_geter(character, ItemLinks, items);
    entity3 spawns[items->length];
    entity e = spawn_window_users_id(world, data, texture, 0, spawns);
    if (!e) {
        zox_logw("Items UI spawning failed.");
        return 0;
    }
    for (int i = 0; i < items->length; i++) {
        entity item = items->value[i];
        entity3 frame = spawns[i];
        if (zox_valid(frame.x)) {
            zox_set(frame.x, ItemLink, { item });
        }
        if (zox_valid(frame.y)) {
            zox_set(frame.y, ItemLink, { item });
        }
        if (zox_valid(frame.z)) {
            zox_set(frame.z, ItemLink, { item });
        }
    }
    return e;
}*/
