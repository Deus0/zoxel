void set_character_action(
    ecs *world,
    entity character,
    byte index)
{
    byte dbg_log = 0;
    if (!zox_valid(character)) {
        if (dbg_log) {
            zox_loge("[set_character_action] Invalid Character");
        }
        return;
    }
    zox_setv(character, ActionIndex, index);
    spawn_sound_from_file_name(
        world,
        prefab_sound,
        "swap_action",
        0,
        get_volume_sfx());
    if (dbg_log) {
        zox_log("[set_character_action] Action Set [%s]:[%i]",
            zox_getn(character),
            index);
    }
    entity player = zox_get_link(
        world,
        character,
        PlayerLink);
    if (!zox_valid(player)) {
        if (dbg_log) {
            zox_loge("[set_character_action] Invalid [player]");
        }
        return;
    }
    entity canvas = zox_get_link(world, player, CanvasLink);
    entity actionbar_ui = zox_get_child_by_id(
        world,
        canvas,
        zox_id(MenuActions));
    if (!zox_valid(actionbar_ui)) {
        zox_loge("[set_character_action] Invalid [actionbar_ui]");
        return;
    }
    entity body = zox_get_child_by_id(
        world,
        actionbar_ui,
        zox_id(WindowBody));
    if (!zox_valid(body)) {
        zox_loge("[set_character_action] Invalid [actionbar_ui.body]");
        return;
    }
    entity body_children[layouts2_children_capacity];
    uint body_children_length = zox_get_children(
        world,
        body,
        body_children,
        layouts2_children_capacity);
    if (body_children_length <= index) {
        zox_loge("[set_character_action] Invalid Frames - ui [%s] - body [%s] - %i (frames) <= %i (index)",
            zox_getn(actionbar_ui),
            zox_getn(body),
            body_children_length,
            index);
    }
    // deselect first
    for (uint i = 0; i < body_children_length; i++) {
        entity child = body_children[i];
        if (!zox_has(child, ActiveState) ||
            !zox_has(child, ChildIndex)
        ) {
            zox_loge("[%i] has no ActiveState", i);
            zox_loge("  - child [%s]", zox_getn(child));
            zox_loge("  - body [%s]", zox_getn(body));
            continue;
        }
        byte state = zox_getv(child, ActiveState);
        uint frame_index = zox_getv(child, ChildIndex);
        if (index != frame_index && state) {
            zox_setv(child, ActiveState, 0);
            zox_setv(child, ActiveStateDirty, zox_dirty_trigger);
        } else if (index == frame_index && !state) {
            zox_setv(child, ActiveState, 1);
            zox_setv(child, ActiveStateDirty, zox_dirty_trigger);
        }
        if (dbg_log) {
            zox_log("[%i] ActiveState", i);
            zox_log("  - child [%s]", zox_getn(child));
            zox_log("  - body [%s]", zox_getn(body));
        }
    }
}


void set_player_action(
    ecs *world,
    entity player,
    byte index)
{
    entity character = zox_get_link(world, player, CharacterLink);
    set_character_action(
        world,
        character,
        index);
}

void player_action_ui_move(
    ecs *world,
    entity player,
    sbyte direction)
{
    entity character = zox_get_link(world, player, CharacterLink);
    if (!zox_valid(character) ||
        !zox_has(character, ActionIndex))
    {
        return;
    }
    entity actionbar = zox_get_child_by_id(
        world,
        character,
        zox_id(Actionbar));
    if (!zox_valid(actionbar)) {
        return;
    }
    uint length = zox_get_children_count(world, actionbar);
    byte index = zox_getv(character, ActionIndex);
    index = index + direction >= 0 ?
        index + direction :
        length + (index + direction);
    if (index >= length) {
        index -= length;
    }
    set_character_action(
        world,
        character,
        index);
}

void button_event_switch_action(ecs *world, ClickEventData event) {
    player_action_ui_move(world, event.clicker, 1);
}

void button_event_switch_action2(ecs *world, ClickEventData event) {
    player_action_ui_move(world, event.clicker, -1);
}

