// NOTE: Using Slots for linking UIs to Data
entity spawn_datagrid3(ecs* world, entity prefab, entity prefab_frame, entity prefab_icon, entity prefab_label, byte label_font_size, entity canvas, entity character, entity slots_manager, byte2 cells_size, const char* header_label, color fill, color outline, float2 position_anchor, int2 position) {
    if (!zox_valid(character)) {
        zox_log_error("invalid character in [spawn_datagrid3]");
        return 0;
    }
    if (!zox_valid(prefab_frame)) {
        zox_loge("[%s] has an invalid prefab frame", header_label);
        return 0;
    }
    // Get our Slots
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(world, slots_manager, slots, layouts2_children_capacity, zox_id(Slot));
    // Get our window data
    zox_geter_value(canvas, LayoutSize, int2, canvas_size);
    color grid_fill = window_fill;
    color grid_outline = window_outline;
    // TODO: Remove these structs
    SpawnWindowUsers data = get_default_datagrid_data(world, prefab, character, canvas, canvas_size);
    // TODO: Calculate Grid Rows/Height based on slots length
    int2 icon_size = int2_single((default_icon_size / 4) * ui_scale);
    int2 frame_size = int2_single((default_frame_size / 4) * ui_scale);
    int2 size = calculate_grid_size(cells_size, frame_size.x, data.window.grid_padding, data.window.grid_margins);
    data.frame.texture.fill_color = fill;
    byte active_states = zox_has(prefab_frame, ActiveState);
    byte selected = 0;
    if (active_states) {
        if (selected >= slots_length) {
            zox_logw("selected [%i] out of bounds [%i]", selected, slots_length);
            selected = slots_length - 1;
        }
    }
    // Spawns Window here!!
    zox_instance(prefab);
    zox_set_unique_name(e, header_label);
    // Spawn the header!!!
    // byte is_header = header_label != NULL;
    byte header_height = 0;
    {
        byte is_close_button = 1;
        byte header_font_size = 6 * ui_scale;
        if (header_label == "") {
            header_font_size = 0;
        }
        byte2 header_margins = (byte2) { 4 * ui_scale, 3 * ui_scale };
        byte header_font_thickness_s = header_font_thickness * ui_scale;
        byte header_fonto_thickness_s = header_font_thickness * ui_scale;
        color header_font_fill = header_font_fill;
        color header_font_outline = header_font_outline;
        header_height = header_font_size + header_margins.y * 2;
        float2 header_anchor = (float2) { 0.5f, 1 };
        int2 header_position = (int2) { 0, -header_height / 2 };
        int2 header_size = (int2) { size.x, header_height };
        spawn_header(world, e, canvas, header_position, header_size, header_anchor, header_label, header_font_size, header_margins, int2_zero, header_size, is_close_button, &on_closed_taskbar_window, canvas_size);
    }
    size.y += header_height;
    initialize_element(world, e, canvas, canvas, position, size, size, position_anchor, 0);
    set_window_bounds_to_canvas(world, e, canvas_size, size, position_anchor);
    int2 grid_size = int2_sub(size, (int2) { 0, header_height });
    int2 grid_position = (int2) { 0, -header_height / 2 };
    // Spawns Grid !!!
    entity grid = spawn_uic(world, prefab_grid, e, float2_half, grid_position, grid_size, grid_size, grid_fill, grid_outline);
    zox_set_unique_name(grid, "window_gridgrid");
    zox_set(grid, GridSize, { cells_size });
    zox_set(grid, GridPadding, { data.window.grid_padding });
    zox_set(grid, GridMargins, { data.window.grid_margins });
    uint array_index = 0;
    for (int j = cells_size.y - 1; j >= 0; j--) {
        for (int i = 0; i < cells_size.x; i++) {
            entity slot = slots[array_index];
            if (!zox_valid(slot) || !zox_has(slot, DataLink)) {
                zox_loge("Invalid [%s] Slot at [%i]", zox_get_name(slots_manager), array_index);
                continue;
            }
            entity dat = zox_gett_value(slot, DataLink);
            entity3 spawn = spawn_frame(world, prefab_frame, prefab_icon, prefab_label, grid, position, frame_size, icon_size, label_font_size, array_index);
            // We can just link icons now
            if (zox_valid(spawn.y)) {
                zox_add_tag(spawn.y, DataFrame);
                zox_set(spawn.y, DataLink, { dat });
                zox_set(spawn.y, SlotLink, { slot });
                zox_set(spawn.y, DataDirty, { zox_dirty_trigger });
                // zox_set_id(spawn.y, link_id, sizeof(entity), dat);
            }
            if (zox_valid(spawn.z)) {
                // zox_set(spawn.z, DataLink, { dat });
                zox_set(spawn.z, SlotLink, { slot });
                // zox_set_id(spawn.z, link_id, sizeof(entity), dat);
            }
            if (active_states) {
                if (array_index == selected) {
                    zox_set(spawn.x, ActiveState, { 1 });
                    zox_set(spawn.x, ActiveStateDirty, { zox_dirty_trigger });
                }
            }
            array_index++;
            if (array_index >= slots_length) {
                break;
            }
        }
    }
    // add to characters element links and link to character
    if (zox_has(character, ElementLinks)) {
        zox_muter(character, ElementLinks, elementLinks);
        add_to_ElementLinks(elementLinks, e);
        zox_set(e, ElementHolder, { character });
    }
    return e;
}
