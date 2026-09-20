// NOTE: When Data dirty, set the icon texture
// TODO: Use SlotLink instead of DataLink, more accurate
// NOTE: If slots data is dirty, we can also update texture

zox_sys2(SlotDataCleanSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_out(DataLink);
    // zox_sys_out(DataDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_o(DataLink, data);
        // zox_sys_o(DataDirty, state);
        if (data->value && !zox_valid(data->value)) {
            data->value = 0;
            // state->value = zox_dirty_trigger;
            zox_add(e, DataDirty);
        }
    }
} zox_sys_end(SlotDataCleanSystem);

// NOTE: Flags dirty if slot data is dirty
zox_sys2(DataIconSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    // zox_sys_in(SlotLink);
    zox_sys_out(DataLink);
    // zox_sys_out(DataDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        // zox_sys_i(SlotLink, slot);
        zox_sys_o(DataLink, data);
        // zox_sys_o(DataDirty, state);
        entity slot = zox_get_link(world, e, SlotLink);
        if (!zox_valid(slot)) {
            continue;
        }
        if (zox_has(slot, DataUpdate)) {
            data->value = zox_getv(slot, DataLink);
            // state->value = zox_dirty_trigger;
            zox_add(e, DataDirty);
            // zox_log("Slot was dirty [%s]", zox_get_name(slot->value));
            if (dbg_log) {
                zox_log("Data UI - Slot UI was Dirty [%s]",
                    zox_sys_e_name);
            }
        }
    }
} zox_sys_end(DataIconSystem);

// NOTE: Updates icon texture when dirty
// NOTE: Also updates the Tooltip to clear it
zox_sys2(DataIconUpdateSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    //zox_sys_in(DataDirty);
    zox_sys_in(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        //zox_sys_i(DataDirty, state);
        zox_sys_i(DataLink, data);
        /*if (state->value != zox_dirty_active) {
            continue;
        }*/
        // set texture of dirty
        entity texture = zox_valid(data->value) ?
            zox_get_link(world, data->value, TextureLink) :
            0;
            // zox_has(data->value, TextureLink) ? zox_getv(data->value, TextureLink) : 0;
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
        if (dbg_log) {
            zox_log("Data Icon Updated [%s]",
                zox_sys_e_name);
        }
    }
} zox_sys_end(DataIconUpdateSystem);
