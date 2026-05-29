color frame_equip_fill = { 63, 43, 33, frame_alpha };
color frame_equip_outline = { 63, 43, 33, frame_alpha };

void fetch_equip_slots_parts(ecs *world, entity_array_d* entities, entity e) {
    if (zox_has(e, EquipSlot)) {
        // add_to_entity_array_d(entities, zox_getv(e, DataLink));
        add_to_entity_array_d(entities, e);
    }
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity slot = slots[k];
        fetch_equip_slots_parts(world, entities, slot);
    }
}

/*entity spawn_player_menu_equipment(ecs* world, entity player) {
    entity prefab = prefab_window; // prefab_menu_body
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    SpawnWindowUsers data = get_default_spawn_window_users_data(world, prefab, character, canvas, canvas_size);
    data.header_zext.text = "Gear";
    data.element.prefab = prefab;
    data.icon.prefab = prefab_icon;
    data.frame.texture.fill_color = default_fill_color_frame_equip;
    FrameTextureData texture = (FrameTextureData) {
        .fill_color = window_fill,
        .outline_color = window_outline
    };
    entity_array_d* parts = create_entity_array_d(1);
    entity chest_slot = zox_get_child_by_id(world, character, zox_id(Body));
    fetch_equip_slots_parts(world, parts, chest_slot);
    // Calculate grid based on slots found
    byte grid_length = next_root(parts->size);
    if (grid_length < 3) {
        grid_length = 3;
    }
    byte header_height = data.header_zext.font_size + data.header_zext.margins.y * 2;
    data.window.grid_size = byte2_single(grid_length);
    data.element.size = calculate_grid_window_size(data.window, header_height);
    // Spawn our window
    entity3 spawns[parts->size];
    entity e = spawn_window_users(world, data, texture, 0, spawns, parts->data, parts->size);
    zox_add_tag(e, MenuEquipment);
    dispose_entity_array_d(parts);
    return e;
}
*/
entity spawn_player_menu_equipment(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    // entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
    byte label_font_size = 5 * ui_scale;
    int frame_size = ((default_frame_size / 4) * ui_scale);
    float2 position_anchor = float2_half;
    int2 position = int2_zero;
    entity frame_id = zox_id(EquipFrame);
    // Calculate grid based on slots found
    entity_array_d* parts = create_entity_array_d(1);
    entity chest_slot = zox_get_child_by_id(world, character, zox_id(Body));
    fetch_equip_slots_parts(world, parts, chest_slot);
    byte grid_length = next_root(parts->size);
    if (grid_length < 4) {
        grid_length = 4;
    }
    byte2 cells_size = byte2_single(grid_length);
    // Spawn our window
    entity e = spawn_datagrid_slots2(world, prefab_window, prefab_frame_selectable, prefab_icon, prefab_label, label_font_size, canvas, character, cells_size, "Gear", frame_equip_fill, frame_equip_outline, position_anchor, position, frame_id, parts->data, parts->size);
    zox_add_tag(e, MenuEquipment);
    dispose_entity_array_d(parts);
    return e;
}

byte is_equip_frame(ecs* world, entity frame) {
    return zox_valid(frame) && zox_has(frame, EquipFrame);
}

byte is_equip_data(ecs* world, entity data) {
    return zox_valid(data) && zox_has(data, EquipItem);
}

void on_frame_updated_equipment(ecs* world, entity body) {
    if (zox_valid(body)) {
       zox_set(body, BodyDirty, { zox_dirty_trigger });
    }
}
