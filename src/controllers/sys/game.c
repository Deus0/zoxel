// When player state changes, for player
zox_sys2(PlayerGameStateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    zox_sys_in(CharacterLink);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        zox_sys_i(CharacterLink, character);
        byte action_type = 0;
        if (dirty->value == zox_dirty_active && state->value == zox_player_state_pause_begin) {
            action_type = 1;
        } else if (dirty->value == zox_dirty_active && state->value == zox_player_state_play_begin) {
            action_type = 2;
        }
        if (action_type) {
            byte can_move = action_type - 1;
            zox_log("Player State Updated [%i]", can_move);
            disable_inputs_until_release(world, e, zox_device_mode_none, 1);
            entity mouse = zox_get_child_by_id(world, e, zox_id(Mouse));
            if (zox_valid(mouse)) {
                zox_set(mouse, MouseLock, { can_move });
            } else {
                zox_loge("No Mouse Found on player.");
            }
            if (zox_valid(character->value)) {
                zox_set(character->value, DisableMovement, { !can_move });
            }
        }
    }
} zox_sys_end(PlayerGameStateSystem);
