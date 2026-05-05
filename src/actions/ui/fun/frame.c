// note on meta: since just spawned user item, we cant use it yet
void on_action_set(ecs* world, entity e, byte index, entity action, entity meta) {
    zox_geter(e, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuActions, menu);

    if (!zox_valid(menu)) {
        return;
    }

    // zox_geter(menu, Children, children);

    entity children[layouts2_children_capacity];
    uint children_length = zox_get_children(world, menu, children, layouts2_children_capacity);
    if (children_length < 2) {
        return;
    }

    entity body = children[1];
    entity body_children[layouts2_children_capacity];
    uint body_children_length = zox_get_children(world, body, body_children, layouts2_children_capacity);
    // zox_geter(body, Children, body_children);

    if (index >= body_children_length) {
        zox_logw("Index [%i] >= UIs [%i]", index, body_children_length);
        return;
    }

    entity frame = body_children[index];
    entity frame_children[layouts2_children_capacity];
    uint frame_children_length = zox_get_children(world, frame, frame_children, layouts2_children_capacity);

    // zox_geter(frame, Children, frame_children);
    entity icon = frame_children[0];
    entity label = frame_children_length >= 1 ? frame_children[1] : 0;
    // remember: uses meta item for texture source here
    // zox_get_prefab(action, meta);
    set_icon_from_user_data(world, frame, icon, meta);
    zox_set(icon, UserDataLink, { action });

    // also set other links
    zox_set(frame, ItemLink, { action });
    zox_set(icon, ItemLink, { action });

    if (label) {
        zox_set(label, ItemLink, { action });
        zox_muter(label, TextData, text_data);
        zox_muter(label, TextDirty, text_dirty);
        dispose_TextData(text_data);
        text_dirty->value = zox_dirty_trigger;
    }
}


void on_action_removed(ecs* world, entity e, entity user) {
    zox_muter(user, ActionLinks, actions);

    int action_index = -1;
    for (int j = 0; j < actions->length; j++) {
        if (actions->value[j] == e) {
            action_index = j;
            break;
        }
    }

    if (action_index == -1) {
        return;
    }

    actions->value[action_index] = 0;

    if (!zox_has(user, ElementLinks)) {
        zox_log_error("Character has no ElementLinks");
        return;
    }
    zox_geter(user, ElementLinks, elements);

    find_array_element_with_tag(elements, MenuActions, actionbar);
    if (!zox_valid(actionbar)) {
        zox_log_error("Character has no actionbar");
        return;
    }

    entity actionbar_children[layouts2_children_capacity];
    uint actionbar_children_length = zox_get_children(world, actionbar, actionbar_children, layouts2_children_capacity);
    // zox_geter(actionbar, Children, menu_actions_children);
    if (actionbar_children_length < 2) {
        zox_log_error("Character has bad menu_actions_children");
        return;
    }

    entity body = actionbar_children[1];
    if (!zox_valid(body)) {
        zox_log_error("invalid menu_actions_body")
        return;
    }

    entity body_children[layouts2_children_capacity];
    uint body_children_length = zox_get_children(world, body, body_children, layouts2_children_capacity);
    // zox_geter(menu_actions_body, Children, menu_actions_body_children);
    if (action_index >= body_children_length) {
        return;
    }

    entity frame = body_children[action_index];
    if (!zox_valid(frame)) {
        zox_log_error("invalid frame_action")
        return;
    }

    entity frame_children[layouts2_children_capacity];
    uint frame_children_length = zox_get_children(world, frame, frame_children, layouts2_children_capacity);
    // zox_geter(frame_action, Children, frame_children);
    if (!frame_children_length ) {
        return;
    }

    entity icon = frame_children[0];
    if (!zox_valid(icon)) {
        zox_log_error("invalid icon_action")
        return;
    }

    // now reset icon
    set_icon_from_user_data(world, frame, icon, 0);
}