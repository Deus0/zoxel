const double exit_combat_time = 15.0;

zox_sys2(CombatStateSystem) {
    zox_sys_begin();
    zox_sys_in(LastCombatTime);
    zox_sys_out(CombatState);
    for (int i = 0; i < it->count; i++) {
        zox_sys_i(LastCombatTime, last_time);
        zox_sys_o(CombatState, state);
        if (state->value == zox_combat_trigger_battle) {
            state->value = zox_combat_enter_battle;
        } else if (state->value == zox_combat_enter_battle) {
            state->value = zox_combat_battle;
        } else if (state->value == zox_combat_battle) {
            double time_since_combat =
                zox_current_time - last_time->value;
            if (time_since_combat >= exit_combat_time) {
                state->value = zox_combat_trigger_leaving;
            }
        } else if (state->value == zox_combat_trigger_leaving) {
            state->value = zox_combat_leaving;
        } else if (state->value == zox_combat_leaving) {
            state->value = zox_combat_peace;
        }
    }
} zox_sys_end(CombatStateSystem);