// Early Exit from Dialogue
zox_sys2(DialogueExitSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionE);
    zox_sys_in(PlayerLink);
    zox_sys_out(DialogueRunLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(TriggerActionE, state);
        zox_sys_i(PlayerLink, player);
        zox_sys_o(DialogueRunLink, run);

        if (state->value != zox_dirty_active) {
            continue;
        }

        zox_geter_value(player->value, PlayerState, byte, player_state);
        if (player_state == zox_player_state_dialogue_active) {
            zox_set(run->value, NodetreeEnd, { zox_dirty_trigger });
            run->value = 0;
        }
    }
} zox_sys_end(DialogueExitSystem);