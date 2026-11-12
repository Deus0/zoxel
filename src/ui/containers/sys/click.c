// TODO: Remove Externs, make use System States
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
void data_ui_click_system(iter* it) {
    zox_sys_on_begin();
    byte dbg_log = 0;
    zox_sys_world();
    zox_sys_begin();
    //zox_sys_in(ClickState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        //zox_sys_i(ClickState, state);
        /*if (state->value != zox_click_state_clicked_this_frame) {
            continue;
        }*/
        entity slot = zox_get_link(world, e, SlotLink);
        if (!slot) {
            continue;
        }
        if (dbg_log) {
            zox_log("DataUI is Activating [%s]", zox_get_name(e));
        }
        entity canvas = zox_get_parent_by_id(world, e, zox_id(Canvas));
        if (!zox_valid(canvas)) {
            continue;
        }
        entity mouse_ui = zox_get_child_by_id(world, canvas, zox_id(DataMouse));
        if (!zox_valid(mouse_ui)) {
            continue;
        }
        entity data = zox_get_link(world, e, DataLink);
        entity mouse_data = zox_get_link(world, mouse_ui, DataLink);
        byte mouse_data_empty = !zox_valid(mouse_data);
        byte clicked_data_empty = !zox_valid(data);
        // If both empty
        if (mouse_data_empty && clicked_data_empty) {
            if (dbg_log) {
                zox_log("   Both Mouse and DataUI are empty.");
            }
            continue;
        }
        // check if can place here
        entity place_in_frame = zox_get_parent(world, e);
        byte is_mouse_equip = is_data_equip_item(world, mouse_data);
        byte is_frame_equip2 = is_frame_equip_item(world, place_in_frame);
        byte is_mouse_body = is_data_body_part(world, mouse_data);
        byte is_frame_body2 = is_frame_body_part(world, place_in_frame);
        if (!is_action_frame(world, place_in_frame)) {
            if (is_frame_equip2) {
                // check if same slot held if swapping
                // if place in or grab out we update body dirty
                if (mouse_data && !is_mouse_equip) {
                    continue;
                }
            } else if (is_frame_body2) {
                if (mouse_data && !is_mouse_body) {
                    continue;
                }
            }
            else if (!can_place_icon_in_skill_frame(world, place_in_frame, mouse_data)) {
                continue;
            } else if (!can_place_icon_in_item_frame(world, place_in_frame, mouse_data)) {
                continue;
            }
        }
        // If One is Empty and one is exists!
        // NOTE: If Mouse picking up data!
        if (mouse_data_empty && !clicked_data_empty) {
            if (dbg_log) {
                zox_log("   Mouse is Empty, DataUI has [%s]", zox_get_name(data));
            }
            if (is_frame_body2) {
                // continue here if child body slots all empty!
                if (!is_slot_children_empty(world, slot)) {
                    zox_log("Cannot Pickup part, children are not empty!");
                    continue;
                }
            }
        }
        // NOTE: If Mouse placing data!
        else if (!mouse_data_empty && clicked_data_empty) {
            if (dbg_log) {
                zox_log("   DataUI is Empty, Mouse has [%s]", zox_get_name(mouse_data));
            }
            if (is_frame_body2) {
                // continue here if child body slots all empty!
                // TODO: Check Part Slot Type!
                if (is_slot_parent_empty(world, slot)) {
                    zox_log("Cannot Place Part, Parent is Partless!");
                    continue;
                }
            }
        }
        // NOTE: If Swapping Data!
        else {
            if (dbg_log) {
                zox_log("   DataUI has [%s], Mouse has [%s]",
                    zox_get_name(data),
                    zox_get_name(mouse_data));
            }
            // check if both base types are the same
            entity base_item_1 = zox_get_prefab(world, mouse_data);
            entity base_item_2 = zox_get_prefab(world, data);
            if (base_item_1 == base_item_2) {
                if (can_stack_items(world, data, mouse_data)) {
                    zox_log("Stacking Items! %s",
                        zox_get_name(base_item_1));
                    stack_items(world, data, mouse_data);
                    zox_set_link(world, mouse_ui, DataLink, 0);
                    zox_setv(mouse_ui, RenderDisabled, 1);
                    // slot dirty so its quantity label updates
                    zox_add(slot, DataDirty);
                    zox_add(e, DataDirty);
                    continue;
                }
            }
        }
        entity any_data = data > 0 ? data : mouse_data;
        entity user = zox_get_parent(world, any_data);
        // NOTE: This Handles Swapping
        entity temp = mouse_data;
        mouse_data = data;
        data = temp;
        zox_set_link(world, mouse_ui, DataLink, mouse_data);
        zox_set_link(world, e, DataLink, data);
        zox_set(mouse_ui, RenderDisabled, { !zox_valid(mouse_data) });
        // we should just set DataDirty here
        swap_textures(world, e, mouse_ui);
        zox_set_link(world, slot, DataLink, data);
        zox_add(slot, DataDirty);
        zox_add(e, DataDirty);
        if (is_frame_equip2 || is_frame_body2) {
            on_frame_updated_equipment(world, user);
        }
    }
    zox_sys_on_end();
} zoxd_system(data_ui_click_system);
