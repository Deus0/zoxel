// When player state changes, for player
zox_sys2(ControllerPlayerStateSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerStateDirty);
    zox_sys_in(PlayerState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerStateDirty, dirty);
        zox_sys_i(PlayerState, state);
        byte action_type = 0;
        if (dirty->value == zox_dirty_active &&
            state->value == zox_player_state_pause_begin)
        {
            action_type = 1;
        } else if (dirty->value == zox_dirty_active &&
            state->value == zox_player_state_play_begin)
        {
            action_type = 2;
        }
        if (action_type) {
            byte can_move = action_type - 1;
            // zox_log("Player State Updated [%i]", can_move);
            disable_inputs_until_release(
                world,
                e,
                zox_device_mode_gamepad);
            entity mouse = zox_get_child_by_id(
                world,
                e,
                zox_id(Mouse));
            if (zox_valid(mouse)) {
                zox_setv(mouse, MouseLock, can_move);
            } else {
                zox_loge("No Mouse Found on player.");
            }
            /*if (!zox_valid(character->value)) {
                continue;
            }
            // do i need this here?
            if (can_move &&
                zox_has(character->value, DisableMovement))
            {
                zox_remove(character->value, DisableMovement);
            } else if (!can_move &&
                !zox_has(character->value, DisableMovement))
            {
                zox_add(character->value, DisableMovement);
            }*/
        }
    }
} zox_sys_end(ControllerPlayerStateSystem);
