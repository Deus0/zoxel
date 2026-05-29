color default_fill_color_frame_body = { 33, 63, 63, frame_alpha };

void fetch_slots_body_parts(ecs *world, entity_array_d* entities, entity e) {
    if (zox_has(e, BodySlot)) {
        add_to_entity_array_d(entities, zox_getv(e, DataLink));
    }
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity slot = slots[k];
        fetch_slots_body_parts(world, entities, slot);
    }
}

entity spawn_player_menu_body(ecs* world, entity player) {
    entity prefab = prefab_window; // prefab_menu_body
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab, character, canvas, canvas_size);
    data.header_zext.text = "Body";
    data.element.prefab = prefab;
    data.icon.prefab = prefab_icon;
    data.frame.texture.fill_color = default_fill_color_frame_body;
    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };
    entity_array_d* parts = create_entity_array_d(1);
    entity chest_slot = zox_get_child_by_id(world, character, zox_id(Body));
    fetch_slots_body_parts(world, parts, chest_slot);
    entity3 spawns[parts->size];
    // Calculate grid based on slots found
    byte grid_length = next_root(parts->size);
    byte header_height = data.header_zext.font_size + data.header_zext.margins.y * 2;
    data.window.grid_size = byte2_single(grid_length);
    data.element.size = calculate_grid_window_size(data.window, header_height);
    // Spawns our grid window
    entity e = spawn_window_users(world, data, texture, 0, spawns, parts->data, parts->size);
    zox_add_tag(e, MenuBody);
    dispose_entity_array_d(parts);
    // zox_log("parts->size [%i]:[%i]", parts->size, grid_length);
    return e;
}
