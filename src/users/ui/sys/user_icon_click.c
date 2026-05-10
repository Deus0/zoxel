extern byte process_icon_type_action(ecs*, const entity);
extern void set_linked_action(ecs*, const entity, const int, const entity);
extern void set_linked_item(ecs*, const entity, const int, const entity);
extern void set_linked_skill(ecs*, const entity, const int, const entity);
extern void link_as_new_item(ecs*, const entity, const entity3);

// Called from the clicked UI
zox_sys2(UserIconClickSystem) {
    if (!icon_mouse_follow) {
        return; // global icon_mouse_follow for now
    }
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(ClickState);
    zox_sys_in(IconType);
    zox_sys_in(IconIndex);
    zox_sys_out(UserDataLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(ClickState, clickState);
        zox_sys_i(IconType, iconType);
        zox_sys_i(IconIndex, iconIndex);
        zox_sys_o(UserDataLink, userDataLink);
        if (clickState->value != zox_click_state_clicked_this_frame) {
            continue;
        }
        byte icon_type = iconType->value;
        if (!icon_type) {
            continue;
        }
        zox_geter_value(icon_mouse_follow, UserDataLink, entity, mouse_data);
        byte mouse_data_empty = !zox_valid(mouse_data);
        byte clicked_data_empty = !zox_valid(userDataLink->value);
        if (mouse_data_empty && clicked_data_empty) {
            continue; // if both empty
        }
        // check matches mouse's icon type
        zox_geter_value(icon_mouse_follow, IconType, byte, mouse_icon_type);
        if (mouse_icon_type > zox_icon_type_action && icon_type > zox_icon_type_action && icon_type != mouse_icon_type) {
            zox_log(" ! cannot place [%i] in [%i] slot\n", mouse_icon_type, icon_type)
            continue; // didn't match
        }
        entity character = 0;
        if (!mouse_data_empty) {
            character = zox_get_value(mouse_data, UserLink);
        } else if (!clicked_data_empty) {
            character = zox_get_value(userDataLink->value, UserLink);
        }
        // what icon frame is clicked?
        // swap with icon_mouse_follow:
        // q: is all data on icon??
        // swap icon with mouse icon
        if (!clicked_data_empty) {
            if (icon_type == zox_icon_type_action) {
                icon_type = process_icon_type_action(world, userDataLink->value);
            }
            zox_set(icon_mouse_follow, IconType, { icon_type });
        } else {
            zox_set(icon_mouse_follow, IconType, { 0 });
        }
        zox_set(icon_mouse_follow, UserDataLink, { userDataLink->value });
        zox_set(icon_mouse_follow, RenderDisabled, { clicked_data_empty });
        userDataLink->value = mouse_data;
        // zox_log("swapping textures\n")
        swap_textures(world, e, icon_mouse_follow);
        // remember: this is a temporary fix for: bug where e doesn't clear on picked up items
        entity frame = zox_get_parent(world, e);
        // zox_geter_value(e, ParentLink, entity, frame);
        if (mouse_data_empty) {
            set_icon_from_user_data(world, frame, e, 0);
        }
        entity frame_children[layouts2_children_capacity];
        uint frame_children_length = zox_get_children(world, frame, frame_children, layouts2_children_capacity);
        entity3 framer = (entity3) { frame, e,
            // fetches the label
            frame_children_length > 1 ? frame_children[1] : 0
        };
        // set_icon_label_from_user_data(world, frame, mouse_data);
        // new data placed in mouse_data
        // use iconType->value and iconIndex->value to set data on character
        // how to get character from icon? UserLink!
        // === Base on Frame clicked ===
        if (iconType->value == zox_icon_type_action) {
            zox_log(" + character [%lu] setting [%s] [%i]", character, "action", iconIndex->value)
            set_linked_action(world, character, iconIndex->value, mouse_data);
            link_as_new_item(world, mouse_data, framer);
        } else if (iconType->value == zox_icon_type_skill) {
            // zox_log(" + character [%lu] setting [%s] [%i]\n", character, "skill", iconIndex->value)
            set_linked_skill(world, character, iconIndex->value, mouse_data);
        } else if (iconType->value == zox_icon_type_item) {
            // zox_log(" + character [%lu] setting [%s] [%i]\n", character, "item", iconIndex->value)
            set_linked_item(world, character, iconIndex->value, mouse_data);
            link_as_new_item(world, mouse_data, framer);
        }
        // clear the tooltip when picked up icon
        if (!clicked_data_empty) {
            zox_geter_value(e, CanvasLink, entity, canvas);
            if (zox_valid(canvas)) {
                entity tooltip = zox_get_child_by_id(world, canvas, zox_id(Tooltip));
                if (zox_valid(tooltip)) {
                    set_entity_text(world, tooltip, "");
                }
            }
        }
    }
} zox_sys_end(UserIconClickSystem);
