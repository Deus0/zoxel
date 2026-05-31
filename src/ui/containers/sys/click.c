// For Stacking Icons
extern byte can_stack_items(ecs*, entity, entity);
extern byte stack_items(ecs*, entity, entity);
extern byte is_action_frame(ecs*, entity);
extern byte can_place_icon_in_skill_frame(ecs*, entity, entity);
extern byte can_place_icon_in_item_frame(ecs*, entity, entity);
// Equipment
extern byte is_frame_equip_item(ecs*, entity);
extern byte is_data_equip_item(ecs*, entity);
extern void on_frame_updated_equipment(ecs*, entity);
// Body
extern byte is_frame_body_part(ecs*, entity);
extern byte is_data_body_part(ecs*, entity);

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
        // check if can place here
        entity place_in_frame = zox_get_parent(world, e);
        byte is_mouse_equip = is_data_equip_item(world, mouse_data->value);
        byte is_frame_equip2 = is_frame_equip_item(world, place_in_frame);
        byte is_mouse_body = is_data_body_part(world, mouse_data->value);
        byte is_frame_body2 = is_frame_body_part(world, place_in_frame);
        if (!is_action_frame(world, place_in_frame)) {
            if (is_frame_equip2) {
                // check if same slot held if swapping
                // if place in or grab out we update body dirty
                if (mouse_data->value && !is_mouse_equip) {
                    continue;
                }
            } else if (is_frame_body2) {
                if (mouse_data->value && !is_mouse_body) {
                    continue;
                }
            }
            else if (!can_place_icon_in_skill_frame(world, place_in_frame, mouse_data->value)) {
                continue;
            } else if (!can_place_icon_in_item_frame(world, place_in_frame, mouse_data->value)) {
                continue;
            }
        }
        // If One is Empty and one is exists!
        // NOTE: If Mouse picking up data!
        if (mouse_data_empty && !clicked_data_empty) {
            if (dbg_log) {
                zox_log("   Mouse is Empty, DataFrame has [%s]", zox_get_name(data->value));
            }
            if (is_frame_body2) {
                // continue here if child body slots all empty!
                if (!is_slot_children_empty(world, slot->value)) {
                    zox_log("Cannot Pickup part, children are not empty!");
                    continue;
                }
            }
        }
        // NOTE: If Mouse placing data!
        else if (!mouse_data_empty && clicked_data_empty) {
            if (dbg_log) {
                zox_log("   DataFrame is Empty, Mouse has [%s]", zox_get_name(mouse_data->value));
            }
            if (is_frame_body2) {
                // continue here if child body slots all empty!
                // TODO: Check Part Slot Type!
                if (is_slot_parent_empty(world, slot->value)) {
                    zox_log("Cannot Place Part, Parent is Partless!");
                    continue;
                }
            }
        }
        // NOTE: If Swapping Data!
        else {
            if (dbg_log) {
                zox_log("   DataFrame has [%s], Mouse has [%s]", zox_get_name(data->value), zox_get_name(mouse_data->value));
            }
            // check if both base types are the same
            entity base_item_1 = zox_get_prefab(world, mouse_data->value);
            entity base_item_2 = zox_get_prefab(world, data->value);
            if (base_item_1 == base_item_2) {
                if (can_stack_items(world, data->value, mouse_data->value)) {
                    zox_log("Stacking Items! %s", zox_get_name(base_item_1));
                    stack_items(world, data->value, mouse_data->value);
                    dirty->value = zox_dirty_trigger;
                    mouse_data->value = 0;
                    zox_mut_end(mouse_ui, DataLink);
                    zox_set(mouse_ui, RenderDisabled, { 1 });
                    continue;
                }
            }
        }
        entity any_data = data->value > 0 ? data->value : mouse_data->value;
        entity user = zox_get_parent(world, any_data);
        // ;
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
        if (is_frame_equip2 || is_frame_body2) {
            on_frame_updated_equipment(world, user);
        }
    }
} zox_sys_end(DataFrameClickSystem);
