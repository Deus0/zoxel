entity spawn_menu_inventory(ecs* world, entity player) {
    zox_geter_value(player, CanvasLink, entity, canvas);
    zox_geter_value(player, CharacterLink, entity, character);
    entity inventory = zox_get_child_by_id(world, character, zox_id(Inventory));
    byte2 cells_size = byte2_single(5);
    byte label_font_size = 5 * ui_scale;
    float2 position_anchor = float2_half;
    int2 position = int2_zero;
    entity frame_id = zox_id(ItemFrame);
    entity e = spawn_datagrid_slots(world, prefab_window, prefab_frame, prefab_icon, prefab_label, label_font_size, canvas, character, inventory, cells_size, "Inventory", default_fill_color_frame_item, default_fill_color_frame_item, position_anchor, position, frame_id);
    zox_add_tag(e, MenuItems);
    return e;
}
