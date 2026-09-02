// Run from DialoguetreeRun
zox_sys2(DialogueEndSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodetreeEnd);
    zox_sys_in(SpeakerLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(NodetreeEnd, state);
        zox_sys_i(SpeakerLinks, speakers);
        // Process on completed tree
        if (state->value != zox_dirty_active) {
            continue;
        }
        entity ui = zox_get_link(world, e, DialogueUI);
        if (zox_valid(ui)) {
            zox_delete(ui);
        }
        if (zox_valid(speakers->value[1])) {
            unfollow(world, speakers->value[1]);
        }
        if (zox_valid(speakers->value[0])) {
            zox_geter_value(speakers->value[0], PlayerLink, entity, player);
            if (zox_valid(player)) {
                zox_set(player, PlayerState, { zox_player_state_dialogue_end });
                entity canvas = zox_get_link(world, player, Canvas);
                entity windows[zox_children_capacity];
                uint length = zox_get_children_by_id(world, canvas, windows, zox_children_capacity, zox_id(Window));
                for (int j = 0; j < length; j++) {
                    entity window = windows[j];
                    set_children_by_id_byte(world, window, zox_id(RenderDisabled), 0);
                }
            }
        }
        /*if (zox_valid(speakers->value[0]) && zox_valid(speakers->value[1])) {
            zox_log("[%s] Stopped Talking to [%s]", zox_get_name(speakers->value[0]), zox_get_name(speakers->value[1]));
        }*/

    }
} zox_sys_end(DialogueEndSystem);
