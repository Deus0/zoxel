// Run from DialoguetreeRun
zox_sys2(DialogueEndSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(NodetreeEnd);
    zox_sys_in(DialogueUILink);
    zox_sys_in(SpeakerLinks);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(NodetreeEnd, state);
        zox_sys_i(DialogueUILink, ui);
        zox_sys_i(SpeakerLinks, speakers);

        // Process on completed tree
        if (state->value != zox_dirty_active) {
            continue;
        }

        if (zox_valid(ui->value)) {
            zox_delete(ui->value);
        }
        if (zox_valid(speakers->value[0])) {
            zox_geter_value(speakers->value[0], PlayerLink, entity, player);
            if (zox_valid(player)) {
                zox_set(player, PlayerState, { zox_player_state_dialogue_end });
            }
        }
        if (zox_valid(speakers->value[1])) {
            unfollow(world, speakers->value[1]);
        }
        /*if (zox_valid(speakers->value[0]) && zox_valid(speakers->value[1])) {
            zox_log("[%s] Stopped Talking to [%s]", zox_get_name(speakers->value[0]), zox_get_name(speakers->value[1]));
        }*/

    }
} zox_sys_end(DialogueEndSystem);