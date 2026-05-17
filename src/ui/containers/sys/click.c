// NOTE: Called from the clicked icon
zox_sys2(DataFrameClickSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    entity mouse_ui = icon_mouse_follow;
    if (!zox_valid(mouse_ui)) {
        zox_loge("mouse_ui is Invalid");
        return; // global mouse_ui for now
    }
    zox_sys_begin();
    zox_sys_in(ClickState);
    zox_sys_in(SlotLink);
    zox_sys_out(DataLink);
    zox_sys_out(DataDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ClickState, state);
        zox_sys_i(SlotLink, slot);
        zox_sys_o(DataLink, data);
        zox_sys_o(DataDirty, dirty);
        // zox_log("%s is data framing", zox_get_name(e));
        if (state->value != zox_click_state_clicked_this_frame) {
            continue;
        }
        if (dbg_log) {
            zox_log("DataFrame is Activating [%s]", zox_get_name(e));
        }
        zox_mut_begin(mouse_ui, DataLink, mouse_data);
        byte mouse_data_empty = !zox_valid(mouse_data->value);
        byte clicked_data_empty = !zox_valid(data->value);
        // If both empty
        if (mouse_data_empty && clicked_data_empty) {
            if (dbg_log) {
                zox_log("   Both Mouse and DataFrame are empty.");
            }
            continue;
        }
        // If One is Empty and one is exists!
        else if (mouse_data_empty && !clicked_data_empty) {
            if (dbg_log) {
                zox_log("   Mouse is Empty, DataFrame has [%s]", zox_get_name(data->value));
            }
        } else if (!mouse_data_empty && clicked_data_empty) {
            if (dbg_log) {
                zox_log("   DataFrame is Empty, Mouse has [%s]", zox_get_name(mouse_data->value));
            }
        }
        // If Both Exist
        else {
            if (dbg_log) {
                zox_log("   DataFrame has [%s], Mouse has [%s]", zox_get_name(data->value), zox_get_name(mouse_data->value));
            }
        }
        // NOTE: This Handles Swapping
        entity temp = mouse_data->value;
        mouse_data->value = data->value;
        data->value = temp;
        zox_mut_end(mouse_ui, DataLink);
        zox_set(mouse_ui, RenderDisabled, { !zox_valid(mouse_data->value) });
        // we should just set DataDirty here
        swap_textures(world, e, mouse_ui);
        zox_muter(slot->value, DataLink, slot_data);
        slot_data->value = data->value;
        dirty->value = zox_dirty_trigger;
        // NOTE: Clears the tooltip when picked up icon
        /*if (!clicked_data_empty) {
            entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
            if (zox_valid(canvas)) {
                entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
                if (zox_valid(tooltip)) {
                    set_entity_text(world, tooltip, "");
                }
            }
        }*/
    }
} zox_sys_end(DataFrameClickSystem);
