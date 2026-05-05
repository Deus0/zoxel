void set_player_action(ecs *world, entity player, byte index) {

    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character)) {
        return;
    }

    zox_set(character, ActionIndex, { index });
    spawn_sound_from_file_name(world, prefab_sound, "swap_action", 0, get_volume_sfx());

    zox_geter(character, ElementLinks, elements);
    find_array_element_with_tag(elements, MenuActions, actionbar);
    if (!actionbar) {
        return;
    }

    entity window_children[layouts2_children_capacity];
    uint window_children_length = zox_get_children(world, actionbar, window_children, layouts2_children_capacity);
    if (window_children_length < 2) {
        return;
    }

    entity body = window_children[1];
    entity body_children[layouts2_children_capacity];
    uint body_children_length = zox_get_children(world, body, body_children, layouts2_children_capacity);

    // deselect first
    for (uint i = 0; i < body_children_length; i++) {
        entity child = body_children[i];

        if (!zox_valid(child)) {
            continue;
        }

        if (!zox_has(child, ActiveState)) {
            zox_loge("[%i] has no ActiveState", i);
            zox_loge("  - child [%s]", zox_get_name(child));
            zox_loge("  - body [%s]", zox_get_name(body));
            continue;
        }

        zox_geter_value(child, ActiveState, byte, state);
        if (index != i && state) {
            zox_set(child, ActiveState, { 0 });
            zox_set(child, ActiveStateDirty, { zox_dirty_trigger });
        } else if (index == i && !state) {
            zox_set(child, ActiveState, { 1 });
            zox_set(child, ActiveStateDirty, { zox_dirty_trigger });
        }
    }
}

void player_action_ui_move(ecs *world, entity player, sbyte direction) {

    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character) || !zox_has(character,  ActionLinks) || !zox_has(character, ActionIndex)) {
        return;
    }

    zox_geter(character, ActionLinks, actions);
    zox_geter_value_non_const(character, ActionIndex, byte, selected);

    selected = selected + direction >= 0 ? selected + direction : actions->length + (selected + direction);
    if (selected >= actions->length) {
        selected -= actions->length;
    }

    zox_set(character, ActionIndex, { selected });
    spawn_sound_from_file_name(world, prefab_sound, "swap_action", 0, get_volume_sfx());

    // now set the ui
    zox_geter_value(player, CanvasLink, entity, canvas);
    // find_child_with_tag(canvas, MenuActions, actionbar);
    entity actionbar = find_child_with_tag2(world, canvas, zox_id(MenuActions));

    if (!actionbar) {
        return;
    }

    entity window_children[layouts2_children_capacity];
    uint window_children_length = zox_get_children(world, actionbar, window_children, layouts2_children_capacity);
    if (window_children_length < 2) {
        return;
    }

    entity body = window_children[1];
    entity body_children[layouts2_children_capacity];
    uint body_children_length = zox_get_children(world, body, window_children, layouts2_children_capacity);

    // deselect any prior ones
    for (uint i = 0; i < body_children_length; i++) {
        entity child = body_children[i];

        if (!zox_valid(child)) {
            continue;
        }
        if (!zox_has(child, ActiveState)) {
            zox_log_error("[%i] has no ActiveState", i);
            continue;
        }
        zox_geter_value(child, ActiveState, byte, state);
        if (selected != i && state) {
            zox_set(child, ActiveState, { 0 });
            zox_set(child, ActiveStateDirty, { zox_dirty_trigger });
        } else if (selected == i && !state) {
            zox_set(child, ActiveState, { 1 });
            zox_set(child, ActiveStateDirty, { zox_dirty_trigger });
        }
    }
}

void button_event_switch_action(ecs *world, ClickEventData event) {
    player_action_ui_move(world, event.clicker, 1);
}
