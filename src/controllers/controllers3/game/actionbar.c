// Spawn the UIs
zox_sys2(PlayerActionbarSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(PlayerState);
    zox_sys_in(PlayerStateDirty);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(PlayerState, state);
        zox_sys_i(PlayerStateDirty, dirty);
        if (state->value == zox_player_state_play_begin && dirty->value == zox_dirty_active) {
            spawn_menu_actions(world, e);
        }
    }
} zox_sys_end(PlayerActionbarSystem);

