void set_character_action(ecs *world, entity character, byte index) {
    if (!zox_valid(character)) {
        return;
    }
    zox_set(character, ActionIndex, { index });
    spawn_sound_from_file_name(world, prefab_sound, "swap_action", 0, get_volume_sfx());
    entity player = zox_getv(character, PlayerLink);
    if (!zox_valid(player)) {
        return;
    }
    entity canvas = zox_getv(player, CanvasLink);
    entity actionbar = zox_get_child_by_id(world, canvas, zox_id(MenuActions));
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
            zox_loge("  - child [%s]", zox_getn(child));
            zox_loge("  - body [%s]", zox_getn(body));
            continue;
        }
        byte state = zox_getv(child, ActiveState);
        if (index != i && state) {
            zox_setv(child, ActiveState, 0);
            zox_setv(child, ActiveStateDirty, zox_dirty_trigger);
        } else if (index == i && !state) {
            zox_setv(child, ActiveState, 1);
            zox_setv(child, ActiveStateDirty, zox_dirty_trigger);
        }
    }
}


void set_player_action(ecs *world, entity player, byte index) {
    zox_geter_value(player, CharacterLink, entity, character);
    set_character_action(world, character, index);
}

void player_action_ui_move(ecs *world, entity player, sbyte direction) {
    zox_geter_value(player, CharacterLink, entity, character);
    if (!zox_valid(character) || !zox_has(character, ActionIndex)) {
        return;
    }
    entity actionbar = zox_get_child_by_id(world, character, zox_id(Actionbar));
    if (!zox_valid(actionbar)) {
        return;
    }
    uint length = zox_get_children_count(world, actionbar);
    zox_geter_value(character, ActionIndex, byte, index);
    index = index + direction >= 0 ? index + direction : length + (index + direction);
    if (index >= length) {
        index -= length;
    }
    set_character_action(world, character, index);
}

void button_event_switch_action(ecs *world, ClickEventData event) {
    player_action_ui_move(world, event.clicker, 1);
}

void button_event_switch_action2(ecs *world, ClickEventData event) {
    player_action_ui_move(world, event.clicker, -1);
}

