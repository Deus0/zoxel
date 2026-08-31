// Early Exit from Dialogue
zox_sys2(DialogueExitSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(TriggerActionE);
    zox_sys_in(PlayerLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(TriggerActionE, state);
        zox_sys_i(PlayerLink, player);
        if (state->value != zox_dirty_active) {
            continue;
        }
        byte player_state = zox_getv(player->value, PlayerState);
        if (player_state == zox_player_state_dialogue_active) {
            entity process = zox_get_link(world, e, DialogueProcess);
            zox_setv(process, NodetreeEnd, zox_dirty_trigger);
            zox_unlink(world, e, DialogueProcess, process);
        }
    }
} zox_sys_end(DialogueExitSystem);
