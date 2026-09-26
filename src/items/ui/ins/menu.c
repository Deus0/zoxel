entity spawn_menu_inventory(ecs* world, entity player) {
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity character = zox_get_link(world, player, CharacterLink);
    entity inventory = zox_get_child_by_id(world, character, zox_id(Inventory));
    byte label_font_size = 5 * ui_scale;
    float2 position_anchor = float2_half;
    int2 position = int2_zero;
    entity frame_id = zox_id(ItemFrame);
    // Calculate grid based on slots found
    byte slots_length = zox_get_children_count(world, inventory);
    byte grid_length = next_root(slots_length);
    if (grid_length < 4) {
        grid_length = 4;
    }
    byte2 cells_size = byte2_single(grid_length);
    // Spawn our window
    entity e = spawn_datagrid_slots(
        world,
        prefab_window,
        prefab_frame,
        prefab_icon,
        prefab_label2,
        label_font_size,
        canvas,
        character,
        inventory,
        cells_size,
        "Items",
        default_fill_color_frame_item,
        default_fill_color_frame_item,
        position_anchor,
        position,
        frame_id,
        0);
    zox_add(e, MenuItems);
    return e;
}
