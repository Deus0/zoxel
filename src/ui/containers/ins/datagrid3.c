// NOTE: Using Slots for linking UIs to Data
entity spawn_datagrid_slots2(
    ecs* world,
    entity prefab,
    entity prefab_frame,
    entity prefab_icon,
    entity prefab_label,
    byte label_font_size,
    entity canvas,
    entity character,
    byte2 cells_size,
    const char* header_label,
    color fill,
    color outline,
    float2 position_anchor,
    int2 position,
    entity frame_id,
    entity* slots,
    uint slots_length)
{
    if (!zox_valid(character)) {
        zox_loge("invalid character in [spawn_datagrid_slots]");
        return 0;
    }
    // TODO: Calculate Grid Rows/Height based on slots length
    byte2 grid_padding = byte2_single(2 * ui_scale);
    byte2 grid_margins = byte2_single(4 * ui_scale);
    int2 icon_size = int2_single((default_icon_size / 4) * ui_scale);
    int2 frame_size = int2_single((default_frame_size / 4) * ui_scale);
    int2 size = calculate_grid_size(
        cells_size,
        frame_size.x,
        grid_padding,
        grid_margins);
    byte active_states = zox_has(prefab_frame, ActiveState);
    byte selected = 0;
    if (active_states) {
        if (selected >= slots_length) {
            zox_logw("selected [%i] out of bounds [%i]", selected, slots_length);
            selected = slots_length - 1;
        }
    }
    // Spawns Window here!!
    byte header_font_size = 8 * ui_scale;
    byte2 header_padding = (byte2) {
        10 * ui_scale,
        4 * ui_scale
    };
    entity3 e3 = spawn_window(
        world,
        prefab,
        prefab_grid,
        header_label,
        canvas,
        position,
        size,
        position_anchor,
        header_font_size,
        header_padding,
        on_closed_taskbar_window);
    entity e = e3.x;
    entity grid = e3.z;
    // zox_set_unique_name(grid, "window_gridg");
    zox_setv(grid, GridSize, cells_size);
    zox_setv(grid, GridPadding, grid_padding);
    zox_setv(grid, GridMargins, grid_margins);
    // Spawn our data frames!
    uint array_index = 0;
    for (int j = cells_size.y - 1; j >= 0; j--) {
        for (int i = 0; i < cells_size.x; i++) {
            entity slot = slots[array_index];
            if (!zox_valid(slot)) {
                zox_loge("[spawn_datagrid] [%s] Invalid Slot [%i]",
                    header_label,
                    array_index);
                array_index++;
                if (array_index >= slots_length) {
                    break;
                }
                continue;
            }
            if (!zox_has(slot, DataLink)) {
                zox_loge("[spawn_datagrid] [%s] Slot [%s] has no DataLink [%i]",
                    header_label,
                    zox_get_name(slot),
                    array_index);
                array_index++;
                if (array_index >= slots_length) {
                    break;
                }
                continue;
            }
            entity dat = zox_getv(slot, DataLink);
            entity3 spawn = spawn_frame(
                world,
                prefab_frame,
                prefab_icon,
                prefab_label2,
                grid,
                position,
                frame_size,
                icon_size,
                label_font_size,
                array_index);
            // We can just link icons now
            entity frame = spawn.x;
            entity icon = spawn.y;
            entity text = spawn.z;
            if (frame) {
                if (zox_valid(frame_id)) {
                    zox_add_id(frame, frame_id);
                }
                zox_setv(frame, ChildIndex, array_index);
            }
            if (icon) {
                zox_add(icon, DataUI);
                zox_add(icon, SlotUser);
                zox_add(icon, DataDirty);
                zox_setv(icon, DataLink, dat);
                zox_link(world, icon, SlotLink, slot);
            }
            if (text) {
                zox_add(text, DataUI);
                zox_add(text, SlotUser);
                zox_add(text, DataDirty);
                zox_link(world, text, SlotLink, slot);
            }
            if (active_states) {
                if (array_index == selected) {
                    zox_setv(spawn.x, ActiveState, 1);
                    zox_setv(spawn.x, ActiveStateDirty, zox_dirty_trigger);
                }
            }
            array_index++;
            if (array_index >= slots_length) {
                break;
            }
        }
        if (array_index >= slots_length) {
            break;
        }
    }
    return e;
}

entity spawn_datagrid_slots(
    ecs* world,
    entity prefab,
    entity prefab_frame,
    entity prefab_icon,
    entity prefab_label,
    byte label_font_size,
    entity canvas,
    entity character,
    entity slots_manager,
    byte2 cells_size,
    const char* header_label,
    color fill,
    color outline,
    float2 position_anchor,
    int2 position,
    entity frame_id)
{
    // Get our Slots
    entity slots[layouts2_children_capacity];
    uint slots_length = zox_get_children_by_id(
        world,
        slots_manager,
        slots,
        layouts2_children_capacity,
        zox_id(Slot));
    return spawn_datagrid_slots2(
        world,
        prefab,
        prefab_frame,
        prefab_icon,
        prefab_label,
        label_font_size,
        canvas,
        character,
        cells_size,
        header_label,
        fill,
        outline,
        position_anchor,
        position,
        frame_id,
        slots,
        slots_length);
}
