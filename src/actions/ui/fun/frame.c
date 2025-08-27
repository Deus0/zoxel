// note on meta: since just spawned user item, we cant use it yet
void on_action_updated(
    ecs* world,
    entity e,
    byte index,
    entity action,
    entity meta
) {
    zox_geter(e, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuActions, ui);
    if (!zox_valid(ui)) {
        return;
    }
    zox_geter(ui, Children, children);
    const entity menu_actions_body = children->value[1];
    zox_geter(menu_actions_body, Children, grand_children);
    const entity frame = grand_children->value[index];
    zox_geter(frame, Children, great_grand_children);
    const entity icon = great_grand_children->value[0];
    // remember: uses meta item for texture source here
    // zox_get_prefab(action, meta);
    set_icon_from_user_data(
        world,
        frame,
        icon,
        meta);
    set_icon_label_from_user_data(
        world,
        frame,
        meta);
    zox_set(icon, UserDataLink, { action });
}


// when action was updated
void on_action_updated_quantity(
    ecs *world,
    const entity character,
    const byte action_selected,
    const byte quantity
) {
    if (!zox_valid(character) || !zox_has(character, ElementLinks)) {
        return;
    }
    zox_geter(character, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuActions, actionbar);
    if (actionbar) {
        zox_geter(actionbar, Children, menu_actions_children);
        const entity menu_actions_body = menu_actions_children->value[1];
        zox_geter(menu_actions_body, Children, menu_actions_body_children);
        const entity frame_action = menu_actions_body_children->value[action_selected];
        set_icon_label_from_user_data_quantity(world, frame_action, quantity);
    }
}

void on_action_updated_quantity2(
    ecs *world,
    const entity e,
    const entity user,
    const byte quantity
) {
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

    on_action_updated_quantity(
        world,
        user,
        action_index,
        quantity
    );
}

void on_action_removed(
    ecs* world,
    entity e,
    entity user
) {
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

    if (zox_has(user, ElementLinks)) {
        zox_geter(user, ElementLinks, elements);

        find_array_element_with_tag(elements, MenuActions, actionbar);
        if (zox_valid(actionbar)) {

            zox_geter(actionbar, Children, menu_actions_children);
            if (menu_actions_children->length >= 2) {

                const entity menu_actions_body = menu_actions_children->value[1];
                if (!zox_valid(menu_actions_body)) {
                    zox_log_error("invalid menu_actions_body")
                    return;
                }
                zox_geter(menu_actions_body, Children, menu_actions_body_children);

                const entity frame_action = menu_actions_body_children->value[action_index];
                if (!zox_valid(frame_action)) {
                    zox_log_error("invalid frame_action")
                    return;
                }

                zox_geter(frame_action, Children, frame_action_children);

                const entity icon_action = frame_action_children->value[0];
                if (!zox_valid(icon_action)) {
                    zox_log_error("invalid icon_action")
                    return;
                }

                // now reset icon
                set_icon_from_user_data(
                    world,
                    frame_action,
                    icon_action,
                    0);
                set_icon_label_from_user_data(
                    world,
                    frame_action,
                    0);
            }
        } else {
            zox_log_error("character has no actionbar")
        }
    } else {
        zox_log_error("character has no element links")
    }
}