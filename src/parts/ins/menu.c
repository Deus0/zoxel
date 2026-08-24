color default_fill_color_frame_body = { 33, 63, 63, frame_alpha };

void fetch_slots_body_parts(ecs *world, entity_array_d* entities, entity e) {
    if (zox_has(e, BodySlot)) {
        entity_array_d_add(entities, e);
    }
    entity slots[zox_children_capacity];
    uint length = zox_get_children_by_id(world, e, slots, zox_children_capacity, zox_id(Slot));
    for (uint k = 0; k < length; k++) {
        entity slot = slots[k];
        fetch_slots_body_parts(world, entities, slot);
    }
}

byte is_frame_body_part(ecs* world, entity frame) {
    return zox_valid(frame) && zox_has(frame, BodyFrame);
}

byte is_data_body_part(ecs* world, entity data) {
    return zox_valid(data) && zox_has(data, BodyItem);
}

entity spawn_player_menu_body(ecs* world, entity player) {
    color frame_equip_fill = { 63, 43, 33, frame_alpha };
    color frame_equip_outline = { 63, 43, 33, frame_alpha };
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    byte label_font_size = 5 * ui_scale;
    // int frame_size = ((default_frame_size / 4) * ui_scale);
    float2 position_anchor = float2_half;
    int2 position = int2_zero;
    entity frame_id = zox_id(BodyFrame);
    // Calculate grid based on slots found
    entity_array_d* parts = create_entity_array_d(1);
    entity chest_slot = zox_get_child_by_id(world, character, zox_id(Body));
    // fetch_equip_slots_parts(world, parts, chest_slot);
    fetch_slots_body_parts(world, parts, chest_slot);
    byte grid_length = next_root(parts->size);
    if (grid_length < 4) {
        grid_length = 4;
    }
    byte2 cells_size = byte2_single(grid_length);
    // Spawn our window
    entity e = spawn_datagrid_slots2(world, prefab_window, prefab_frame, prefab_icon, prefab_label2, label_font_size, canvas, character, cells_size, "Body", frame_equip_fill, frame_equip_outline, position_anchor, position, frame_id, parts->data, parts->size);
    zox_add(e, MenuBody);
    dispose_entity_array_d(parts);
    return e;
}
