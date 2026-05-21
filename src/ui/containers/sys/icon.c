// NOTE: When Data dirty, set the icon texture
// TODO: Use SlotLink instead of DataLink, more accurate
// NOTE: If slots data is dirty, we can also update texture

zox_sys2(SlotDataCleanSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(DataLink);
    zox_sys_out(DataDirty);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_o(DataLink, data);
        zox_sys_o(DataDirty, state);
        if (data->value && !zox_valid(data->value)) {
            data->value = 0;
            state->value = zox_dirty_trigger;
        }
    }
} zox_sys_end(SlotDataCleanSystem);

// NOTE: Flags dirty if slot data is dirty
zox_sys2(DataIconSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(SlotLink);
    zox_sys_out(DataLink);
    zox_sys_out(DataDirty);
    for (int i = 0; i < it->count; i++) {
        // zox_sys_e();
        zox_sys_i(SlotLink, slot);
        zox_sys_o(DataLink, data);
        zox_sys_o(DataDirty, state);
        if (!zox_valid(slot->value)) {
            continue;
        }
        if (zox_gett_value(slot->value, DataDirty) == zox_dirty_active) {
            data->value = zox_gett_value(slot->value, DataLink);
            state->value = zox_dirty_trigger;
            // zox_log("Slot was dirty [%s]", zox_get_name(slot->value));
        }
    }
} zox_sys_end(DataIconSystem);

// NOTE: Updates icon texture when dirty
zox_sys2(DataIconUpdateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(DataDirty);
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(DataDirty, state);
        zox_sys_i(DataLink, data);
        if (state->value != zox_dirty_active) {
            continue;
        }
        // set texture of dirty
        entity texture = zox_valid(data->value) && zox_has(data->value, TextureLink) ? zox_gett_value(data->value, TextureLink) : 0;
        if (!texture) {
            if (zox_valid(data->value)) {
                texture = string_hashmap_get(files_hashmap_textures, new_string_data("blank"));
            } else {
                texture = string_hashmap_get(files_hashmap_textures, new_string_data("empty"));
            }
        }
        clone_texture_data(world, e, texture);
        // zox_log("Set Icon [%s] to texture [%s]", zox_get_name(e), zox_get_name(texture));if (!clicked_data_empty) {
        // For now lets do this
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (zox_valid(canvas)) {
            entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
            if (zox_valid(tooltip)) {
                set_entity_text(world, tooltip, "");
            }
        }
    }
} zox_sys_end(DataIconUpdateSystem);
