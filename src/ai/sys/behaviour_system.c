zox_sys2(BehaviourSystem) {
    zox_sys_world();
    zox_sys_begin();
    zox_sys_in(CombatState);
    zox_sys_in(DefaultBehaviour);
    zox_sys_out(Behaviour);
    zox_sys_out(MoveForwards);
    zox_sys_out(RotateTowards);
    zox_sys_out(MoveSpeed);
    for (int i = 0; i < it->count; i++) {
        zox_sys_e();
        zox_sys_i(CombatState, combat);
        zox_sys_i(DefaultBehaviour, default_behaviour);
        zox_sys_o(Behaviour, behaviour);
        zox_sys_o(MoveForwards, move);
        zox_sys_o(RotateTowards, rotate);
        zox_sys_o(MoveSpeed, move_speed);

        move->value = behaviour->value != zox_behaviour_idle;
        rotate->value = behaviour->value != zox_behaviour_idle;
        // only flee if in “wander” mode and not frozen
        if (combat->value == zox_combat_battle) {
            const byte is_coward = zox_has(e, Coward);
            if (is_coward && behaviour->value != zox_behaviour_flee) {
                behaviour->value = zox_behaviour_flee;
                move_speed->value = 8;
                if (is_debug_behaviour) {
                    zox_log("+ has started to flee [%s]", zox_get_name(e))
                }
            } else if (!is_coward && behaviour->value != zox_behaviour_attack) {
                behaviour->value = zox_behaviour_attack;
                move_speed->value = 6;
                if (is_debug_behaviour) {
                    zox_log("+ has started to attack [%s]", zox_get_name(e))
                }
            }
        } else if (combat->value == zox_combat_peace) {
            if (behaviour->value != zox_behaviour_follow &&
                behaviour->value != default_behaviour->value) {
                behaviour->value = default_behaviour->value;
                /*if (is_debug_behaviour) {
                    zox_log("+ has started to [%i] [%s]", default_behaviour->value, zox_get_name(e))
                }*/
            }
            move_speed->value = 3;
        }
    }
} zox_sys_end(BehaviourSystem);