// Called from the clicked UI
zox_sys2(DataFrameClickSystem) {
    byte dbg_log = 0;
    zox_sys_world();
    entity mouse_ui = icon_mouse_follow;
    if (!zox_valid(mouse_ui)) {
        zox_loge("mouse_ui is Invalid");
        return; // global mouse_ui for now
    }
    // zox_log("Data Framing: %i", it->count);
    zox_sys_begin();
    zox_sys_in(ClickState);
    zox_sys_out(DataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ClickState, state);
        zox_sys_o(DataLink, data);
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
        if (mouse_data_empty && clicked_data_empty) {
            if (dbg_log) {
                zox_log("   Both Mouse and DataFrame are empty.");
            }
            continue; // if both empty
        } else if (mouse_data_empty && !clicked_data_empty) {
            if (dbg_log) {
                zox_log("   Mouse is Empty, DataFrame has [%s]", zox_get_name(data->value));
            }

        } else if (!mouse_data_empty && clicked_data_empty) {
            if (dbg_log) {
                zox_log("   DataFrame is Empty, Mouse has [%s]", zox_get_name(mouse_data->value));
            }
        }
        // Assume Swapping
        entity temp = mouse_data->value;
        mouse_data->value = data->value;
        data->value = temp;
        zox_mut_end(mouse_ui, DataLink);
        zox_set(mouse_ui, RenderDisabled, { !zox_valid(mouse_data->value) });
        // we should just set DataDirty here
        swap_textures(world, e, mouse_ui);
        // entity frame = zox_get_parent(world, e);
        // zox_muter(e, DataLink, data);
        // frame_data->value = data->value;
        entity slot = zox_gett_value(e, SlotLink);
        zox_muter(slot, DataLink, slot_data);
        slot_data->value = data->value;
        // clear the tooltip when picked up icon
        if (!clicked_data_empty) {
            entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
            if (zox_valid(canvas)) {
                entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
                if (zox_valid(tooltip)) {
                    set_entity_text(world, tooltip, "");
                }
            }
        }
    }
} zox_sys_end(DataFrameClickSystem);
